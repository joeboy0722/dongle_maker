#include "dongle_reader.h"
#include "../common/crypto_helper.h"
#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <winioctl.h>
#include <initguid.h>
#include <usbioctl.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

// 定義磁碟類別 GUID (GUID_DEVINTERFACE_DISK)
DEFINE_GUID(GUID_DEVINTERFACE_DISK_READER, 0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);

// 用於解密的 Salt (必須與 Maker 端完全相同)
const std::string DEVELOPER_SALT = "joeboy0722_DongleSystem";

// 1. 安全讀取當前 USB 的出廠晶片唯讀序號 (完全不需要系統管理員權限)
DONGLE_API bool GetUsbHardwareId(int driveNumber, char* serialOut, int maxLen) {
    if (serialOut == nullptr || maxLen <= 0) return false;

    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_DISK_READER, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) return false;

    SP_DEVICE_INTERFACE_DATA interfaceData = { sizeof(interfaceData) };
    bool found = false;

    for (DWORD dwIndex = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_DISK_READER, dwIndex, &interfaceData); dwIndex++) {
        DWORD dwRequiredSize = 0;
        SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, NULL, 0, &dwRequiredSize, NULL);
        
        std::vector<BYTE> detailBuffer(dwRequiredSize);
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)detailBuffer.data();
        pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

        SP_DEVINFO_DATA devInfoData = { sizeof(devInfoData) };
        if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, pDetail, dwRequiredSize, NULL, &devInfoData)) {
            continue;
        }

        // 以免權限的 0 模式開啟磁碟，僅為獲取其實體磁碟代號 (此操作在普通權限下保證成功)
        HANDLE hDevice = CreateFileW(pDetail->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice == INVALID_HANDLE_VALUE) continue;

        STORAGE_DEVICE_NUMBER deviceNumber = { 0 };
        DWORD bytesReturned = 0;
        BOOL isDisk = DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &deviceNumber, sizeof(deviceNumber), &bytesReturned, NULL);
        CloseHandle(hDevice);

        // 如果這不是我們指定的實體磁碟代號，跳過
        if (!isDisk || (int)deviceNumber.DeviceNumber != driveNumber) {
            continue;
        }

        // 走父層節點鏈獲取 USB 晶片唯一序號
        std::string serialNumber = "";
        DEVINST devInstParent = 0;
        if (CM_Get_Parent(&devInstParent, devInfoData.DevInst, 0) == CR_SUCCESS) {
            DEVINST devInstUsb = 0;
            if (CM_Get_Parent(&devInstUsb, devInstParent, 0) == CR_SUCCESS) {
                WCHAR usbId[MAX_PATH] = { 0 };
                if (CM_Get_Device_IDW(devInstUsb, usbId, MAX_PATH, 0) == CR_SUCCESS) {
                    std::wstring wUsbId = usbId;
                    size_t pos = wUsbId.find_last_of(L'\\');
                    if (pos != std::wstring::npos) {
                        std::wstring wSerial = wUsbId.substr(pos + 1);
                        size_t ampPos = wSerial.find(L'&');
                        if (ampPos != std::wstring::npos) {
                            wSerial = wSerial.substr(0, ampPos);
                        }
                        serialNumber.assign(wSerial.begin(), wSerial.end());
                    }
                }
            }
        }

        if (!serialNumber.empty()) {
            strcpy_s(serialOut, maxLen, serialNumber.c_str());
            found = true;
            break;
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return found;
}

// 2. 自動定位無磁碟機代號的隱形分割區，讀取加密內容並在記憶體中還原為明文 (由呼叫端傳入自訂密碼解密)
DONGLE_API bool ReadHiddenData(int driveNumber, const char* password, char* dataOut, int maxLen) {
    if (dataOut == nullptr || password == nullptr || maxLen <= 0) return false;

    // 1. 先安全獲取當前 USB 的晶片序號
    char serial[256] = { 0 };
    if (!GetUsbHardwareId(driveNumber, serial, 256)) {
        return false;
    }

    // 2. 搜尋該隨身碟上的隱藏 Volume GUID (起始位址必須在 1MB 偏移對齊點)
    std::wstring targetVolumeGuid = L"";
    WCHAR volumeName[MAX_PATH] = { 0 };
    HANDLE hVolEnum = FindFirstVolumeW(volumeName, MAX_PATH);
    if (hVolEnum == INVALID_HANDLE_VALUE) return false;

    do {
        std::wstring volPath = volumeName;
        if (!volPath.empty() && volPath.back() == L'\\') {
            volPath.pop_back();
        }

        // 以 0 權限開啟 Volume GUID 以查詢其硬體屬性 (免管理員權限)
        HANDLE hVolume = CreateFileW(volPath.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hVolume != INVALID_HANDLE_VALUE) {
            VOLUME_DISK_EXTENTS extents = { 0 };
            DWORD bytesReturned = 0;
            BOOL success = DeviceIoControl(hVolume, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &extents, sizeof(extents), &bytesReturned, NULL);
            if (success && extents.NumberOfDiskExtents > 0) {
                // 比對實體磁碟代號與 1MB 起始對齊位址 (符合我們建立隱藏分割區的設定)
                if (extents.Extents[0].DiskNumber == (DWORD)driveNumber && 
                    extents.Extents[0].StartingOffset.QuadPart == 1024LL * 1024LL) {
                    targetVolumeGuid = volumeName; // 找到對應的 Volume
                    CloseHandle(hVolume);
                    break;
                }
            }
            CloseHandle(hVolume);
        }
    } while (FindNextVolumeW(hVolEnum, volumeName, MAX_PATH));

    FindVolumeClose(hVolEnum);

    if (targetVolumeGuid.empty()) {
        return false;
    }

    // 3. 直接以 Volume GUID 路徑唯讀打開隱藏分割區內部的 license.dat (免除磁碟機代號掛載)
    std::wstring filePath = targetVolumeGuid + L"license.dat";
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    // 讀取加密資料
    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE || fileSize == 0) {
        CloseHandle(hFile);
        return false;
    }

    std::vector<uint8_t> ciphertext(fileSize);
    DWORD bytesRead = 0;
    BOOL readOk = ReadFile(hFile, ciphertext.data(), fileSize, &bytesRead, NULL);
    CloseHandle(hFile);

    if (!readOk || bytesRead != fileSize) {
        return false;
    }

    // 4. 衍生 AES 金鑰並解密資料
    std::vector<uint8_t> key;
    if (!CryptoHelper::DeriveKey(serial, password, key)) {
        return false;
    }

    std::string plaintext = "";
    if (!CryptoHelper::DecryptAES256(ciphertext, key, plaintext)) {
        return false; // 如果被複製到別台隨身碟，此步驟將因為序號不符而失敗或還原出亂碼
    }

    // 5. 複製解密後的資料回傳給呼叫端
    if (plaintext.size() >= (size_t)maxLen) {
        return false; // 緩衝區不足
    }

    strcpy_s(dataOut, maxLen, plaintext.c_str());
    return true;
}
