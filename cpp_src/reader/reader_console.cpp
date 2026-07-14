#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <setupapi.h>
#include <shlobj.h>
#include <initguid.h>
#include "dongle_reader.h"

// 我們在此以手動方式掃描磁碟以利主控台展示，Reader 專案不需要包含 Maker.dll。
// 為了簡化展示，我們使用與 Maker 端相同結構的手動 SetupAPI 掃描，這在普通使用者權限下是 100% 可行的。
DEFINE_GUID(GUID_DEVINTERFACE_DISK_READER_CONSOLE, 0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);

void ListCurrentUsbs() {
    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_DISK_READER_CONSOLE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) return;

    SP_DEVICE_INTERFACE_DATA interfaceData = { sizeof(interfaceData) };
    std::cout << "\n[系統偵測] 目前插入的 USB 磁碟清單 (普通使用者權限)：" << std::endl;

    for (DWORD dwIndex = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_DISK_READER_CONSOLE, dwIndex, &interfaceData); dwIndex++) {
        DWORD dwRequiredSize = 0;
        SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, NULL, 0, &dwRequiredSize, NULL);
        
        std::vector<BYTE> detailBuffer(dwRequiredSize);
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)detailBuffer.data();
        pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

        SP_DEVINFO_DATA devInfoData = { sizeof(devInfoData) };
        if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, pDetail, dwRequiredSize, NULL, &devInfoData)) {
            continue;
        }

        HANDLE hDevice = CreateFileW(pDetail->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice == INVALID_HANDLE_VALUE) continue;

        STORAGE_DEVICE_NUMBER deviceNumber = { 0 };
        DWORD bytesReturned = 0;
        BOOL isDisk = DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &deviceNumber, sizeof(deviceNumber), &bytesReturned, NULL);
        
        STORAGE_PROPERTY_QUERY query = {};
        query.PropertyId = StorageDeviceProperty;
        query.QueryType = PropertyStandardQuery;
        std::vector<BYTE> propBuffer(1024);
        BOOL isUsb = FALSE;
        std::string modelName = "Generic USB Device";

        if (isDisk && DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), propBuffer.data(), (DWORD)propBuffer.size(), &bytesReturned, NULL)) {
            PSTORAGE_DEVICE_DESCRIPTOR desc = (PSTORAGE_DEVICE_DESCRIPTOR)propBuffer.data();
            if (desc->BusType == BusTypeUsb) {
                isUsb = TRUE;
                std::string vendor = "";
                std::string product = "";
                if (desc->VendorIdOffset > 0 && desc->VendorIdOffset < bytesReturned) {
                    vendor = (char*)(propBuffer.data() + desc->VendorIdOffset);
                }
                if (desc->ProductIdOffset > 0 && desc->ProductIdOffset < bytesReturned) {
                    product = (char*)(propBuffer.data() + desc->ProductIdOffset);
                }
                auto trim = [](std::string& s) {
                    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
                    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
                };
                trim(vendor);
                trim(product);
                if (!vendor.empty() || !product.empty()) {
                    modelName = vendor + " " + product;
                }
            }
        }
        CloseHandle(hDevice);

        if (isUsb) {
            std::cout << " -> 實體磁碟號: PhysicalDrive" << deviceNumber.DeviceNumber 
                      << " (" << modelName << ")" << std::endl;
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
}

int main() {
    // 設定主控台輸出為 UTF-8
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== USB 軟體鎖讀取工具 (客戶端執行環境 - 完全免權限) ===" << std::endl;
    std::cout << "[*] 當前執行權限：" << (IsUserAnAdmin() ? "系統管理員 (UAC ON)" : "一般使用者 (UAC OFF)") << std::endl;

    // 1. 列出目前所有的 USB 磁碟以利使用者選擇
    ListCurrentUsbs();

    std::cout << "\n請輸入目標 USB 磁碟號 (例如 PhysicalDrive1 請輸入 1): ";
    int driveNumber = -1;
    std::cin >> driveNumber;
    if (driveNumber < 0) {
        std::cout << "[!] 磁碟號無效。" << std::endl;
        return 0;
    }

    // 2. 呼叫 DongleReader.dll API 讀取 USB 晶片序號
    char serial[256] = { 0 };
    std::cout << "\n[*] 正在讀取 USB 晶片硬體唯一序號..." << std::endl;
    if (GetUsbHardwareId(driveNumber, serial, sizeof(serial))) {
        std::cout << "[+] 成功獲取硬體序號: " << serial << std::endl;
    } else {
        std::cout << "[!] 無法讀取該磁碟的 USB 晶片序號，可能該磁碟不是 USB 介面。" << std::endl;
        return 0;
    }

    // 3. 呼叫 DongleReader.dll API 定位隱藏分割區並讀取解密資料
    std::string inputPwd;
    std::cout << "\n請輸入製鎖密碼以進行解密: ";
    std::cin >> inputPwd;

    char decryptedData[1024] = { 0 };
    std::cout << "\n[*] 正在尋找隱形分割區並讀取加密資料..." << std::endl;
    if (ReadHiddenData(driveNumber, inputPwd.c_str(), decryptedData, sizeof(decryptedData))) {
        std::cout << "[+] 授權解密成功！" << std::endl;
        std::cout << "[+] 讀取到的解密明文 (JSON)：" << std::endl;
        std::cout << "-----------------------------------------------" << std::endl;
        std::cout << decryptedData << std::endl;
        std::cout << "-----------------------------------------------" << std::endl;
    } else {
        std::cout << "[!] 讀取或解密失敗！可能原因：" << std::endl;
        std::cout << "    1. 隨身碟未被製作成隱形軟體鎖" << std::endl;
        std::cout << "    2. 授權檔不存在" << std::endl;
        std::cout << "    3. 隨身碟已被完整拷貝 Clone，晶片唯獨序號不符合 (防拷觸發)" << std::endl;
    }

    std::cout << "\n請按任意鍵結束程式..." << std::endl;
    std::cin.get();
    std::cin.get();
    return 0;
}
