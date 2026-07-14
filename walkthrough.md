# USB 隱形授權鎖 (Dongle) 系統整合與開發指引手冊

本手冊詳細說明雙 DLL 授權鎖系統的架構、API 調用規範以及如何將讀取端整合至您的商業主程式中。

---

## 1. 系統運作原理與架構

本系統採用**「雙 DLL 分權設計」**與**「隱形磁軌隱藏技術」**：

* **製作端 (libDongleMaker.dll)**：用於您的工作室。需要系統管理員權限（UAC），可對隨身碟進行清除並建立「雙分割區」：第一個是 128MB 的隱形授權區（類型為 `0x12 OEM` FAT32，用於寫入 AES 加密金鑰並自動卸載隱藏），第二個是佔用剩餘全部空間的可見儲存區（exFAT，標籤為 `USB_DRIVE`，供日常儲存使用）。
* **讀取端 (libDongleReader.dll)**：隨您的主程式一起出貨給客戶。**完全不需要系統管理員權限**，利用 SetupAPI 讀取晶片 ID，並直接透過 Volume GUID 存取隱形分區中的 `license.dat` 進行解密讀取。

```
[製鎖階段 (高權限)]
您的 JSON 授權 + 自訂密碼 ──> libDongleMaker.dll ──> 寫入 USB 隱形磁區 (FAT32)

[驗證階段 (免權限/客戶端)]
您的主程式 ──> 呼叫 libDongleReader.dll (傳入驗證密碼) ──> 解密讀取 JSON 授權 ──> 檢查晶片 ID 防拷
```

---


## 1.5 雙分割區 (Storage + Dongle) 的隱形運作機制

為了讓隨身碟在客戶電腦上插入時「既能當普通隨身碟使用，又不會顯示軟體鎖分割區」，系統會將磁碟重組為以下結構：
1. **隱形鎖區 (Partition 1)**: 
   - 大小：128MB。
   - 類型：`0x12 OEM` (隨身碟在格式化時會被分配 `DONGLE` 標籤，但在燒錄完成前程式會呼叫 API 自動將磁碟機代號拔除)。
   - 當隨身碟拔插時，Windows 看到隨身碟上存在第二個「正常分割區」，會優先為其指派磁碟代號，並因第一分割區的 `0x12` 屬性而**徹底忽略且隱藏它**。
2. **可見儲存區 (Partition 2)**: 
   - 大小：隨身碟扣除 129MB 後的**剩餘最大可用容量**。
   - 格式：相容性最廣的 **exFAT** (標籤為 `USB_DRIVE`)，支援單檔案大於 4GB，支援 Windows 與 macOS 讀寫，讓隨身碟維持原本的日常儲存功能。


## 2. DLL API 函數宣告與說明

### 2.1 製鎖端 (libDongleMaker.dll) - 開發者專用

#### ① 掃描可用的隨身碟
```cpp
// 掃描電腦上所有的實體 USB 隨身碟，填入 infoList 陣列中，回傳掃描到的數量
DONGLE_API int ScanAvailableUsb(UsbDeviceInfo* infoList, int maxDevices);
```

#### ② 清除並建立隱形分區
```cpp
// 對指定的實體隨身碟 (如 PhysicalDrive3) 進行 MBR 隱形分區規劃 (128MB)
// 回傳 0 代表成功，失敗則回傳 Win32 錯誤碼 (如 5 代表 Access Denied，50087 代表參數錯誤)
DONGLE_API int CreateHiddenPartition(int driveNumber);
```

#### ③ 加密並寫入授權資料
```cpp
// 將 JSON 明文字串使用「自訂密碼」與「隨身碟序號」加密後，寫入隨身碟的隱形分區中
DONGLE_API bool WriteHiddenData(int driveNumber, const char* configData, const char* password);
```

---

### 2.2 讀取端 (libDongleReader.dll) - 客戶端出貨專用

#### ① 安全讀取晶片唯讀 ID (免 UAC)
```cpp
// 安全讀取指定實體磁碟的出廠晶片唯讀序號
DONGLE_API bool GetUsbHardwareId(int driveNumber, char* serialOut, int maxLen);
```

#### ② 定位並解密授權資料 (免 UAC)
```cpp
// 定位隱形 Volume GUID 區塊，利用「自訂密碼」解密還原出 JSON 授權字典
DONGLE_API bool ReadHiddenData(int driveNumber, const char* password, char* dataOut, int maxLen);
```

---

## 3. C# (P/Invoke) 整合與聲明範例

將以下宣告放至您的主程式驗證模組中：

```csharp
using System;
using System.Runtime.InteropServices;
using System.Text;

public class DongleSecurity
{
    private const string ReaderDll = "libDongleReader.dll";

    // USB 資訊結構體 (與 C++ 共用記憶體排布)
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct UsbDeviceInfo
    {
        public int DriveNumber;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
        public string SerialNumber;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
        public string Model;
        public ulong Size;
    }

    // 導入讀取晶片序號的 API (免權限)
    [DllImport(ReaderDll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern bool GetUsbHardwareId(int driveNumber, StringBuilder serialOut, int maxLen);

    // 導入解密讀取隱藏資料的 API (免權限)
    [DllImport(ReaderDll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern bool ReadHiddenData(int driveNumber, string password, StringBuilder dataOut, int maxLen);
}
```

---

## 4. 主程式啟動驗證實戰範例 (整合邏輯)

以下為您展示如何使用 `libDongleReader.dll` 在主程式啟動時，自動搜尋插入的隨身碟並執行防拷與授權層級（基本/進階/最高）驗證：

```csharp
using System;
using System.Text;
using System.Web.Script.Serialization; // 需參考 System.Web.Extensions
using System.Collections.Generic;

public class LicenseManager
{
    // 您在 GUI 燒錄製鎖時設定的解密密碼
    private const string SECURITY_PASSWORD = "joeboy0722_DongleSystem";

    public struct LicenseStatus
    {
        public bool IsValid;        // 授權是否有效
        public int UserLevel;       // 權限等級 (Basic=1, Advanced=2, Admin=3)
        public string LicenseUser;  // 被授權人姓名
        public string ErrorMessage; // 錯誤原因
    }

    /// <summary>
    /// 自動搜尋並驗證隨身碟軟體鎖
    /// </summary>
    public static LicenseStatus VerifyDongle()
    {
        LicenseStatus status = new LicenseStatus();
        status.IsValid = false;
        status.UserLevel = 0;

        // 遍歷實體磁碟 1 到 8 (Windows 隨身碟掛載通常在此區間)
        for (int driveNum = 1; driveNum <= 8; driveNum++)
        {
            // 1. 嘗試讀取晶片序號
            StringBuilder serialSb = new StringBuilder(256);
            if (!DongleSecurity.GetUsbHardwareId(driveNum, serialSb, serialSb.Capacity))
            {
                continue; // 讀不到晶片 ID 代表該磁碟不是 USB 裝置，跳過
            }

            string chipSerial = serialSb.ToString();

            // 2. 嘗試使用自訂密碼解密隱藏分割區
            StringBuilder jsonSb = new StringBuilder(4096);
            if (!DongleSecurity.ReadHiddenData(driveNum, SECURITY_PASSWORD, jsonSb, jsonSb.Capacity))
            {
                continue; // 解密失敗，代表這不是針對此軟體製鎖的隨身碟
            }

            // 3. 成功解密出 JSON 明文！開始解析授權字典
            string jsonText = jsonSb.ToString();
            try
            {
                var serializer = new JavaScriptSerializer();
                var dict = serializer.Deserialize<Dictionary<string, object>>(jsonText);

                // 安全防拷二重核對：驗證 JSON 內部寫入的密碼是否吻合
                if (!dict.ContainsKey("password") || dict["password"].ToString() != SECURITY_PASSWORD)
                {
                    status.ErrorMessage = "授權金鑰核對失敗！(內部密碼不符)";
                    return status;
                }

                // 提取權限層級
                if (dict.ContainsKey("level"))
                {
                    status.UserLevel = Convert.ToInt32(dict["level"]);
                }
                
                if (dict.ContainsKey("user"))
                {
                    status.LicenseUser = dict["user"].ToString();
                }

                // 驗證過期時間 (可選)
                if (dict.ContainsKey("expires"))
                {
                    DateTime expireDate = DateTime.Parse(dict["expires"].ToString());
                    if (DateTime.Now > expireDate)
                    {
                        status.ErrorMessage = "授權已過期！過期日：" + expireDate.ToString("yyyy-MM-dd");
                        return status;
                    }
                }

                // 所有核對無誤，授權驗證通過！
                status.IsValid = true;
                status.ErrorMessage = "驗證成功";
                return status;
            }
            catch (Exception ex)
            {
                status.ErrorMessage = "解析授權資料時出錯: " + ex.Message;
                return status;
            }
        }

        status.ErrorMessage = "未偵測到有效的隨身碟軟體鎖，請插入您的授權加密隨身碟。";
        return status;
    }
}
```

---

## 5. 出貨打包指南

當您的軟體準備發行給客戶時，**只需將以下檔案放在與您的主程式 EXE 相同的資料夾下**：

```
[客戶安裝資料夾]
 ├── 您的商業主程式.exe (調用上述 VerifyDongle 方法)
 └── libDongleReader.dll (免管理員權限讀取核心)
```

> [!WARNING]
> * **嚴禁外流 `libDongleMaker.dll`**：該 DLL 包含高風險格式化與清除磁區代碼，僅供您在工作室內燒錄隨身碟使用，切勿隨產品出貨，以確保客戶電腦的安全。
