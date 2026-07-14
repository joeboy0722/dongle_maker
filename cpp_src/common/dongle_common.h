#pragma once

// 定義導出與導入巨集
#ifdef DONGLE_EXPORTS
#define DONGLE_API extern "C" __declspec(dllexport)
#else
#define DONGLE_API extern "C" __declspec(dllimport)
#endif

// USB 裝置資訊結構體
struct UsbDeviceInfo {
    int DriveNumber;             // 實體磁碟代號 (例如 1 代表 PhysicalDrive1)
    char SerialNumber[256];      // 晶片唯獨硬體序號
    char Model[256];             // 裝置型號名稱
    unsigned long long Size;     // 磁碟總容量 (Bytes)
};
