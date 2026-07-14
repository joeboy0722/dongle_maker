#include <iostream>
#include <vector>
#include <string>
#include "dongle_maker.h"
#include <windows.h>

int main() {
    // 設定主控台輸出為 UTF-8
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== 隱形 USB 軟體鎖製作工具 (開發者專用 - 需要管理員權限) ===" << std::endl;

    // 1. 掃描隨身碟
    UsbDeviceInfo devices[10];
    int count = ScanAvailableUsb(devices, 10);
    if (count == 0) {
        std::cout << "[!] 未偵測到符合條件的 USB 隨身碟。請確認已插入隨身碟且執行身分具有管理員權限。" << std::endl;
        std::cout << "請按任意鍵結束..." << std::endl;
        std::cin.get();
        return 0;
    }

    std::cout << "\n偵測到以下 USB 隨身碟：" << std::endl;
    for (int i = 0; i < count; i++) {
        double gbSize = (double)devices[i].Size / (1024.0 * 1024.0 * 1024.0);
        std::cout << "[" << i << "] 實體磁碟號: PhysicalDrive" << devices[i].DriveNumber 
                  << " | 型號: " << devices[i].Model 
                  << " | 容量: " << gbSize << " GB"
                  << " | 晶片序號: " << devices[i].SerialNumber << std::endl;
    }

    std::cout << "\n請選擇要製鎖的 USB 索引號 (0-" << (count - 1) << "): ";
    int choice = -1;
    std::cin >> choice;
    if (choice < 0 || choice >= count) {
        std::cout << "[!] 選擇無效，程式即將結束。" << std::endl;
        return 0;
    }

    UsbDeviceInfo target = devices[choice];

    std::cout << "\n[警告] 您選擇對 PhysicalDrive" << target.DriveNumber << " (" << target.Model << ") 進行製鎖。" << std::endl;
    std::cout << "[警告] 此操作會「抹除該隨身碟上所有的資料」，並重建分割區！" << std::endl;
    std::cout << "確定要繼續嗎？請輸入 'Y' 以確認進行：";
    char confirm = 'N';
    std::cin >> confirm;
    if (confirm != 'Y' && confirm != 'y') {
        std::cout << "[*] 操作已取消。" << std::endl;
        return 0;
    }

    // 2. 建立隱藏分割區
    std::cout << "\n[*] 正在清除磁碟並建立隱形分割區 (格式化為 FAT32)..." << std::endl;
    int err = CreateHiddenPartition(target.DriveNumber);
    if (err != 0) {
        std::cout << "[!] 建立隱形分割區失敗！錯誤碼: " << err << std::endl;
        return 0;
    }
    std::cout << "[+] 隱形分割區建立並格式化成功！" << std::endl;

    // 3. 寫入授權 JSON
    std::string testJson = "{\"level\": 3, \"user\": \"admin\", \"version\": \"ultimate\", \"expires\": \"2029-12-31\"}";
    std::cout << "[*] 正在加密並寫入自訂授權資料..." << std::endl;
    std::cout << "寫入的明文資料為: " << testJson << std::endl;

    bool writeOk = WriteHiddenData(target.DriveNumber, testJson.c_str(), "DefaultPassword123");
    if (!writeOk) {
        std::cout << "[!] 加密寫入資料失敗！" << std::endl;
        return 0;
    }

    std::cout << "[+] 授權資料加密寫入成功！隨身碟已成功轉換為隱形軟體鎖。" << std::endl;
    std::cout << "\n請按任意鍵結束程式..." << std::endl;
    std::cin.get();
    std::cin.get();
    return 0;
}
