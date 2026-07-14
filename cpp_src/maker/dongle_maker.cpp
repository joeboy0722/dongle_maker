#include "dongle_maker.h"
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
DEFINE_GUID(GUID_DEVINTERFACE_DISK_MAKER, 0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);

// 用於加密的 Salt
const std::string DEVELOPER_SALT = "joeboy0722_DongleSystem";

// 內部輔助函式：根據磁碟代號與分割區代號尋找對應的 Volume GUID (例如 "\\\\?\\Volume{GUID}\\")
std::wstring FindVolumeGuidForDiskPartition(int driveNumber, int partitionNumber) {
    WCHAR volumeName[MAX_PATH] = { 0 };
    HANDLE hVolEnum = FindFirstVolumeW(volumeName, MAX_PATH);
    if (hVolEnum == INVALID_HANDLE_VALUE) return L"";

    do {
        std::wstring volPath = volumeName;
        // 去除結尾的反斜線，以便呼叫 CreateFileW
        if (!volPath.empty() && volPath.back() == L'\\') {
            volPath.pop_back();
        }

        // 以 0 權限打開 volume
        HANDLE hVolume = CreateFileW(volPath.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hVolume != INVALID_HANDLE_VALUE) {
            VOLUME_DISK_EXTENTS extents = { 0 };
            DWORD bytesReturned = 0;
            BOOL success = DeviceIoControl(hVolume, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &extents, sizeof(extents), &bytesReturned, NULL);
            if (success && extents.NumberOfDiskExtents > 0) {
                // 比對實體磁碟代號
                if (extents.Extents[0].DiskNumber == (DWORD)driveNumber) {
                    // 比對分割區編號
                    PARTITION_INFORMATION_EX partInfo = {};
                    BOOL gotPart = DeviceIoControl(hVolume, IOCTL_DISK_GET_PARTITION_INFO_EX, NULL, 0, &partInfo, sizeof(partInfo), &bytesReturned, NULL);
                    if (gotPart) {
                        // 在 MBR 下比對 PartitionNumber
                        if (partInfo.PartitionNumber == (DWORD)partitionNumber) {
                            CloseHandle(hVolume);
                            FindVolumeClose(hVolEnum);
                            return volumeName; // 回傳包含尾部反斜線的完整路徑
                        }
                    }
                }
            }
            CloseHandle(hVolume);
        }
    } while (FindNextVolumeW(hVolEnum, volumeName, MAX_PATH));

    FindVolumeClose(hVolEnum);
    return L"";
}

// 內部輔助函式：透過建立子行程執行 Windows 內建 format.exe 進行 FAT32 快速格式化
bool FormatVolumeFAT32(const std::wstring& volumeGuidPath) {
    // format.exe 不接受尾部的反斜線，例如 "\\\\?\\Volume{GUID}"
    std::wstring cmdVolume = volumeGuidPath;
    if (!cmdVolume.empty() && cmdVolume.back() == L'\\') {
        cmdVolume.pop_back();
    }

    // 組成格式化指令：快速格式化為 FAT32，磁碟標籤設為 DONGLE，免互動 (/Y)
    std::wstring commandLine = L"format.exe " + cmdVolume + L" /FS:FAT32 /Q /V:DONGLE /Y";
    
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; // 隱藏主控台視窗

    BOOL success = CreateProcessW(
        NULL,
        &commandLine[0],
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (!success) return false;

    // 等待格式化完成
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return (exitCode == 0);
}

// 1. 掃描電腦上所有的實體 USB 隨身碟，將資訊填入 infoList 陣列中，回傳掃描到的數量
DONGLE_API int ScanAvailableUsb(UsbDeviceInfo* infoList, int maxDevices) {
    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_DISK_MAKER, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) return 0;

    SP_DEVICE_INTERFACE_DATA interfaceData = { sizeof(interfaceData) };
    int usbCount = 0;

    for (DWORD dwIndex = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_DISK_MAKER, dwIndex, &interfaceData); dwIndex++) {
        DWORD dwRequiredSize = 0;
        SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, NULL, 0, &dwRequiredSize, NULL);
        
        std::vector<BYTE> detailBuffer(dwRequiredSize);
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)detailBuffer.data();
        pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

        SP_DEVINFO_DATA devInfoData = { sizeof(devInfoData) };
        if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, pDetail, dwRequiredSize, NULL, &devInfoData)) {
            continue;
        }

        // 開啟磁碟以獲取磁碟代號 (僅做控制，不需讀寫權限)
        HANDLE hDevice = CreateFileW(pDetail->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice == INVALID_HANDLE_VALUE) continue;

        STORAGE_DEVICE_NUMBER deviceNumber = { 0 };
        DWORD bytesReturned = 0;
        BOOL isDisk = DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &deviceNumber, sizeof(deviceNumber), &bytesReturned, NULL);
        
        unsigned long long diskSize = 0;
        if (isDisk) {
            DISK_GEOMETRY_EX geom = {};
            if (DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0, &geom, sizeof(geom), &bytesReturned, NULL)) {
                diskSize = geom.DiskSize.QuadPart;
            }
        }
        
        // 檢查是否是 USB 匯流排
        STORAGE_PROPERTY_QUERY query = {};
        query.PropertyId = StorageDeviceProperty;
        query.QueryType = PropertyStandardQuery;
        std::vector<BYTE> propBuffer(1024);
        BOOL isUsb = FALSE;
        std::string modelName = "Generic USB";
        
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

        if (!isUsb) continue;

        // 取得 USB 晶片唯一的硬體序號 (遍歷 SetupAPI 父層節點)
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
                        // 去除 Windows 自動添加的連接埠後綴 (如 &0 或 &1)
                        size_t ampPos = wSerial.find(L'&');
                        if (ampPos != std::wstring::npos) {
                            wSerial = wSerial.substr(0, ampPos);
                        }
                        serialNumber.assign(wSerial.begin(), wSerial.end());
                    }
                }
            }
        }

        // 如果讀不到實體序號，則不適用於防拷系統
        if (serialNumber.empty()) {
            continue;
        }

        // 磁碟大小已在上方獲取

        if (usbCount < maxDevices) {
            infoList[usbCount].DriveNumber = deviceNumber.DeviceNumber;
            strcpy_s(infoList[usbCount].SerialNumber, serialNumber.c_str());
            strcpy_s(infoList[usbCount].Model, modelName.c_str());
            infoList[usbCount].Size = diskSize;
            usbCount++;
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return usbCount;
}

// 內部輔助函式：鎖定並卸載特定實體磁碟上的所有目前 Volume，以防 Windows 阻擋底層分區覆寫
std::vector<HANDLE> LockAndDismountAllVolumes(int driveNumber) {
    std::vector<HANDLE> lockedHandles;
    WCHAR volumeName[MAX_PATH] = { 0 };
    HANDLE hVolEnum = FindFirstVolumeW(volumeName, MAX_PATH);
    if (hVolEnum == INVALID_HANDLE_VALUE) return lockedHandles;

    do {
        std::wstring volPath = volumeName;
        if (!volPath.empty() && volPath.back() == L'\\') {
            volPath.pop_back();
        }

        // 以 GENERIC_READ | GENERIC_WRITE 權限開啟，以便進行 Lock/Dismount
        HANDLE hVolume = CreateFileW(volPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hVolume != INVALID_HANDLE_VALUE) {
            VOLUME_DISK_EXTENTS extents = { 0 };
            DWORD bytesReturned = 0;
            BOOL success = DeviceIoControl(hVolume, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, &extents, sizeof(extents), &bytesReturned, NULL);
            if (success && extents.NumberOfDiskExtents > 0) {
                if (extents.Extents[0].DiskNumber == (DWORD)driveNumber) {
                    // 該磁區位於我們要處理的實體隨身碟上，進行強制卸載與鎖定
                    DeviceIoControl(hVolume, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &bytesReturned, NULL);
                    if (DeviceIoControl(hVolume, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &bytesReturned, NULL)) {
                        lockedHandles.push_back(hVolume); // 保持控制碼開啟以維護鎖定狀態
                    } else {
                        CloseHandle(hVolume);
                    }
                } else {
                    CloseHandle(hVolume);
                }
            } else {
                CloseHandle(hVolume);
            }
        }
    } while (FindNextVolumeW(hVolEnum, volumeName, MAX_PATH));

    FindVolumeClose(hVolEnum);
    return lockedHandles;
}

// 2. 對指定的實體隨身碟進行清除、重新分割 (回傳 0 代表成功，失敗則回傳 Win32 錯誤代碼)
DONGLE_API int CreateHiddenPartition(int driveNumber) {
    // A. 鎖定並卸載所有存在於該隨身碟上的 Volume
    std::vector<HANDLE> lockedVolumes = LockAndDismountAllVolumes(driveNumber);

    std::wstring drivePath = L"\\\\.\\PhysicalDrive" + std::to_wstring(driveNumber);
    HANDLE hDisk = CreateFileW(drivePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDisk == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        for (HANDLE h : lockedVolumes) CloseHandle(h);
        return (err == 0) ? -1 : (int)err;
    }

    DWORD bytesReturned = 0;

    // 允許擴充 DASD 存取 (強制向驅動程式要求直接讀寫 raw 磁區)
    DeviceIoControl(hDisk, FSCTL_ALLOW_EXTENDED_DASD_IO, NULL, 0, NULL, 0, &bytesReturned, NULL);
    
    // 鎖定實體磁碟
    DeviceIoControl(hDisk, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &bytesReturned, NULL);

    // 2. 清除舊分割表並初始化磁碟為 MBR
    CREATE_DISK createDisk = {};
    createDisk.PartitionStyle = PARTITION_STYLE_MBR;
    createDisk.Mbr.Signature = 0x87654321;
    BOOL ok = DeviceIoControl(hDisk, IOCTL_DISK_CREATE_DISK, &createDisk, sizeof(createDisk), NULL, 0, &bytesReturned, NULL);
    if (!ok) {
        DWORD err = GetLastError();
        DeviceIoControl(hDisk, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &bytesReturned, NULL);
        CloseHandle(hDisk);
        for (HANDLE h : lockedVolumes) CloseHandle(h);
        return (err == 0) ? -2 : (int)err;
    }

    // 3. 設定分割區佈局：建立一個大小為 16MB 的分割區 (MBR 格式必須填寫 4 個 entries)
    DWORD layoutSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 3 * sizeof(PARTITION_INFORMATION_EX);
    std::vector<BYTE> layoutBuf(layoutSize, 0);
    PDRIVE_LAYOUT_INFORMATION_EX pLayout = (PDRIVE_LAYOUT_INFORMATION_EX)layoutBuf.data();
    pLayout->PartitionStyle = PARTITION_STYLE_MBR;
    pLayout->PartitionCount = 4;
    pLayout->Mbr.Signature = 0x87654321;

    // 設定第一個分割區 (隱形分割區)
    pLayout->PartitionEntry[0].PartitionStyle = PARTITION_STYLE_MBR;
    pLayout->PartitionEntry[0].StartingOffset.QuadPart = 1024 * 1024; // 1MB 偏移對齊
    pLayout->PartitionEntry[0].PartitionLength.QuadPart = 16LL * 1024LL * 1024LL; // 16MB
    pLayout->PartitionEntry[0].PartitionNumber = 1;
    pLayout->PartitionEntry[0].RewritePartition = TRUE;
    pLayout->PartitionEntry[0].Mbr.PartitionType = 0x12; // OEM 隱藏分割區 (一般 Windows 檔案總管不分配磁碟機代號且無法直視)
    pLayout->PartitionEntry[0].Mbr.BootIndicator = FALSE;
    pLayout->PartitionEntry[0].Mbr.RecognizedPartition = TRUE;

    // 其餘三個設為空分割區
    for (int i = 1; i < 4; i++) {
        pLayout->PartitionEntry[i].PartitionStyle = PARTITION_STYLE_MBR;
        pLayout->PartitionEntry[i].Mbr.PartitionType = 0; // Empty
    }

    ok = DeviceIoControl(hDisk, IOCTL_DISK_SET_DRIVE_LAYOUT_EX, pLayout, layoutSize, NULL, 0, &bytesReturned, NULL);
    DWORD layoutErr = ok ? 0 : GetLastError();
    
    // 釋放實體磁碟鎖
    DeviceIoControl(hDisk, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &bytesReturned, NULL);
    CloseHandle(hDisk);

    // B. 關閉所有舊 Volume 的控制碼以釋放 Volume 鎖，讓 Plug & Play 系統得以建立新 Volume
    for (HANDLE h : lockedVolumes) {
        DeviceIoControl(h, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &bytesReturned, NULL);
        CloseHandle(h);
    }

    if (!ok) {
        return (layoutErr == 0) ? -3 : (int)layoutErr;
    }

    // C. 重新開啟實體磁碟，強制更新磁碟屬性讓系統重置 Volume 狀態
    hDisk = CreateFileW(drivePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDisk != INVALID_HANDLE_VALUE) {
        DeviceIoControl(hDisk, IOCTL_DISK_UPDATE_PROPERTIES, NULL, 0, NULL, 0, &bytesReturned, NULL);
        CloseHandle(hDisk);
    }

    // 4. 等待 3 秒讓 Windows Plug & Play 建立 Volume 節點
    Sleep(3000);

    // 5. 搜尋新分割區產生的 Volume GUID
    std::wstring volGuidPath = FindVolumeGuidForDiskPartition(driveNumber, 1);
    if (volGuidPath.empty()) {
        return 1001; // 找不到對應的 Volume GUID (可能 PnP 太慢或建立失敗)
    }

    // 6. 將此隱形 Volume 快速格式化為 FAT32
    if (!FormatVolumeFAT32(volGuidPath)) {
        return 1002; // 格式化失敗
    }

    return 0; // 成功
}

// 3. 將自訂資料加密後，寫入目標隨身碟的隱形分割區中 (由呼叫端傳入自訂密碼)
DONGLE_API bool WriteHiddenData(int driveNumber, const char* configData, const char* password) {
    if (configData == nullptr || password == nullptr) return false;

    // 1. 取得該 USB 唯獨序號，作為加密金鑰依據
    // 先掃描獲取對應的序號
    UsbDeviceInfo usbList[10];
    int count = ScanAvailableUsb(usbList, 10);
    std::string serial = "";
    for (int i = 0; i < count; i++) {
        if (usbList[i].DriveNumber == driveNumber) {
            serial = usbList[i].SerialNumber;
            break;
        }
    }

    if (serial.empty()) {
        return false;
    }

    // 2. 衍生密鑰並進行 AES-256-CBC 加密 (使用自訂密碼進行 Key 衍生)
    std::vector<uint8_t> key;
    if (!CryptoHelper::DeriveKey(serial, password, key)) {
        return false;
    }

    std::vector<uint8_t> ciphertext;
    if (!CryptoHelper::EncryptAES256(configData, key, ciphertext)) {
        return false;
    }

    // 3. 尋找隱藏分割區對應的 Volume GUID
    std::wstring volGuidPath = FindVolumeGuidForDiskPartition(driveNumber, 1);
    if (volGuidPath.empty()) {
        return false;
    }

    // 4. 使用 Windows Temp 目錄建立一個臨時的資料夾掛載點，免去佔用全域磁碟機代號 (例如 Z:)
    WCHAR tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring mountDir = std::wstring(tempPath) + L"DongleTempMount\\";
    
    // 建立目錄 (若已存在會失敗，但沒關係)
    CreateDirectoryW(mountDir.c_str(), NULL);

    // 掛載隱形 Volume 到此目錄
    BOOL mounted = SetVolumeMountPointW(mountDir.c_str(), volGuidPath.c_str());
    if (!mounted) {
        RemoveDirectoryW(mountDir.c_str());
        return false;
    }

    // 5. 寫入加密授權檔案 (license.dat)
    std::wstring filePath = mountDir + L"license.dat";
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, NULL);
    bool writeSuccess = false;
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten = 0;
        writeSuccess = WriteFile(hFile, ciphertext.data(), (DWORD)ciphertext.size(), &bytesWritten, NULL);
        CloseHandle(hFile);
    }

    // 6. 卸載臨時掛載點並刪除暫存目錄
    DeleteVolumeMountPointW(mountDir.c_str());
    RemoveDirectoryW(mountDir.c_str());

    return writeSuccess;
}
