#pragma once
#include "../common/dongle_common.h"

// 1. 掃描電腦上所有的實體 USB 隨身碟，將資訊填入 infoList 陣列中，回傳掃描到的數量
DONGLE_API int ScanAvailableUsb(UsbDeviceInfo* infoList, int maxDevices);

// 2. 對指定的實體隨身碟進行清除、重新分割 (建立一個 MBR 隱藏 OEM 分割區，大小約 16MB)
DONGLE_API bool CreateHiddenPartition(int driveNumber);

// 3. 將自訂資料加密後，寫入目標隨身碟的隱形分割區中 (由呼叫端傳入自訂密碼)
DONGLE_API bool WriteHiddenData(int driveNumber, const char* configData, const char* password);
