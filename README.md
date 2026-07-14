# USB 隱形授權系統操作與 C# 整合手冊

本系統為雙 DLL 分權設計，用以解決 Windows 平台下「製鎖端需高風險磁碟權限」與「客戶端要求免 UAC、防毒軟體不誤殺」的商業衝突。

---

## 1. 系統目錄與檔案說明

* **[cpp_src/](file:///d:/dongle_maker/cpp_src)**：所有 C++ 核心 DLL 專案原始碼。
  * **[cpp_src/CMakeLists.txt](file:///d:/dongle_maker/cpp_src/CMakeLists.txt)**：CMake 建置設定。
  * **[cpp_src/common/](file:///d:/dongle_maker/cpp_src/common)**：共用標頭檔與 AES-256 加解密輔助模組 (Windows CNG 實作)。
  * **[cpp_src/maker/](file:///d:/dongle_maker/cpp_src/maker)**：開發端製鎖 DLL 與測試程式（需要管理員權限）。
  * **[cpp_src/reader/](file:///d:/dongle_maker/cpp_src/reader)**：客戶端讀鎖 DLL 與測試程式（完全免 UAC 權限，隨軟體出貨）。
  * **[cpp_src/build/](file:///d:/dongle_maker/cpp_src/build)**：編譯產出目錄，內含編譯出的 DLL 與測試 EXE。
* **[dongle_maker_gui/](file:///d:/dongle_maker/dongle_maker_gui)**：C# WinForms 專案目錄（包含視覺化製鎖燒錄工具）。
  * **[dongle_maker_gui/Form1.cs](file:///d:/dongle_maker/dongle_maker_gui/Form1.cs)**：GUI 主視窗邏輯（USB 掃描、防呆警告、呼叫 C++ DLL）。
  * **[dongle_maker_gui/Form1.Designer.cs](file:///d:/dongle_maker/dongle_maker_gui/Form1.Designer.cs)**：WinForms 介面排版。

---

## 2. 建置與編譯指南

### 第一步：建置 C++ 核心 DLL 庫
使用 CMake 與您的 C++ 編譯器（如 MinGW x64 或 MSVC x64）在 `cpp_src` 下進行編譯：
```powershell
cd d:\dongle_maker\cpp_src
cmake -B build
cmake --build build --config Release
```
> [!NOTE]
> C++ 建置腳本已設定**建置後事件 (Post-Build Event)**。當 C++ 編譯成功後，會自動將 `libDongleMaker.dll` 複製到 C# 專案的輸出目錄 `dongle_maker_gui/bin/Debug/` 與 `dongle_maker_gui/bin/Release/` 中，無需手動複製。

### 第二步：建置 C# GUI 專案
開啟 Visual Studio 載入 `dongle_maker_gui.sln`：
1. **關鍵設定**：由於 C++ DLL 是 64 位元（x64），請確認 `dongle_maker_gui` 專案設定為 **x64 目標平台**（在專案屬性中停用「優先使用 32 位元 / Prefer 32-bit」），否則會發生 `BadImageFormatException` 錯誤。
2. 直接點擊「建置方案 (Build Solution)」即可編譯出 `dongle_maker_gui.exe` 可執行檔。

---

## 3. 燒錄與讀取測試流程

### 步驟 A：使用 C# GUI 進行視覺化製鎖
1. 插入準備作為軟體鎖的 USB 隨身碟。
2. 以**系統管理員身分執行**您的 C# GUI 程式 `dongle_maker_gui.exe`。
3. 程式啟動後會自動掃描 USB 裝置並填入下拉選單。
4. 您可在文字框中修改預設的授權 JSON 字典（例如 `{"level": 3, "user": "admin"}`）。
5. 點擊下方的 **「抹除磁碟並寫入隱形授權鎖」** 紅色按鈕。
6. 程式會跳出**雙重對話框防呆確認**。確認無誤後，即會自動抹除該隨身碟分割區、建立隱形分割區，並將 JSON 加密寫入。

### 步驟 B：客戶端驗證 (唯讀解密)
1. 將剛才製好的 USB 隨身碟插入電腦。
2. 以**一般使用者身分** (不需要管理員權限，無 UAC 提示) 開啟終端機。
3. 切換至讀取目錄並執行 `ReaderConsole.exe`：
   ```powershell
   cd d:\dongle_maker\cpp_src\build\reader
   .\ReaderConsole.exe
   ```
4. 輸入目標隨身碟的磁碟代號。
5. 程式會安全讀取硬體序號、尋找 Volume GUID 並動態解密，正確還原出您在 GUI 寫入的 JSON 授權資料。
