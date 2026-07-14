#pragma once
#include "../common/dongle_common.h"

// 1. 安全讀取當前 USB 的出廠晶片唯讀序號 (完全不需要系統管理員權限)
DONGLE_API bool GetUsbHardwareId(int driveNumber, char* serialOut, int maxLen);

// 2. 自動定位無磁碟機代號的隱形分割區，讀取加密內容並在記憶體中還原為明文 (由呼叫端傳入自訂密碼解密)
DONGLE_API bool ReadHiddenData(int driveNumber, const char* password, char* dataOut, int maxLen);
