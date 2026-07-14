using System;
using System.Collections.Generic;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace dongle_maker_gui
{
    public partial class Form1 : Form
    {
        // 導入 DongleMaker.dll 中的 C++ 導出 API
        [DllImport("libDongleMaker.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern int ScanAvailableUsb([Out] UsbDeviceInfo[] infoList, int maxDevices);

        [DllImport("libDongleMaker.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CreateHiddenPartition(int driveNumber);

        [DllImport("libDongleMaker.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        public static extern bool WriteHiddenData(int driveNumber, string configData, string password);

        public Form1()
        {
            InitializeComponent();
        }

        // 表單載入初始化
        private void Form1_Load(object sender, EventArgs e)
        {
            // 設定預設製鎖密碼
            txtPassword.Text = "joeboy0722_DongleSystem";

            // 設定預設 JSON 範例範本
            txtJsonData.Text = "{\r\n  \"level\": 3,\r\n  \"user\": \"admin\",\r\n  \"version\": \"ultimate\",\r\n  \"expires\": \"2029-12-31\"\r\n}";
            
            // 執行第一次 USB 掃描
            RefreshUsbList();
        }

        // 點選重新整理按鈕
        private void btnRefresh_Click(object sender, EventArgs e)
        {
            RefreshUsbList();
        }

        // 掃描並重新整理 USB 裝置清單
        private void RefreshUsbList()
        {
            cboUsbs.Items.Clear();
            lblStatus.Text = "正在掃描隨身碟...";
            lblStatus.ForeColor = Color.Blue;
            this.Cursor = Cursors.WaitCursor;

            try
            {
                UsbDeviceInfo[] devices = new UsbDeviceInfo[10];
                int count = ScanAvailableUsb(devices, 10);

                if (count > 0)
                {
                    for (int i = 0; i < count; i++)
                    {
                        cboUsbs.Items.Add(new UsbDeviceItem(devices[i]));
                    }
                    cboUsbs.SelectedIndex = 0;
                    lblStatus.Text = $"掃描完成，偵測到 {count} 個符合條件的 USB 裝置。";
                    lblStatus.ForeColor = Color.DarkGreen;
                }
                else
                {
                    lblStatus.Text = "未偵測到任何 USB 隨身碟。";
                    lblStatus.ForeColor = Color.OrangeRed;
                }
            }
            catch (Exception ex)
            {
                lblStatus.Text = "掃描時出錯，請確認 libDongleMaker.dll 存在於目錄下。";
                lblStatus.ForeColor = Color.Red;
                MessageBox.Show($"錯誤資訊: {ex.Message}", "掃描錯誤", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        // 點選燒錄按鈕事件 (高安全 UAC 磁碟清除寫入)
        private void btnBurn_Click(object sender, EventArgs e)
        {
            // 1. 確認已選取隨身碟
            var selectedItem = cboUsbs.SelectedItem as UsbDeviceItem;
            if (selectedItem == null)
            {
                MessageBox.Show("請先選擇要製鎖的 USB 隨身碟！", "操作提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            UsbDeviceInfo target = selectedItem.Info;

            // 2. 驗證密碼欄位
            string password = txtPassword.Text.Trim();
            if (string.IsNullOrEmpty(password))
            {
                MessageBox.Show("自訂製鎖密碼不能為空！此密碼是軟體鎖授權金鑰計算的關鍵。", "欄位空白", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            // 3. 驗證自訂資料欄位是否為合法 JSON 格式，並取得詳細的錯誤位置
            string jsonText = txtJsonData.Text.Trim();
            string jsonError = "";
            if (!IsValidJson(jsonText, out jsonError))
            {
                MessageBox.Show($"授權資料格式必須是合法的 JSON！\n\n[詳細解析錯誤]\n{jsonError}", "JSON 語法錯誤", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            // 4. 解析 JSON 並自動注入密碼欄位，組合成最終要寫入的加密 JSON 資料
            string finalJsonToWrite = "";
            try
            {
                var serializer = new System.Web.Script.Serialization.JavaScriptSerializer();
                var dict = serializer.Deserialize<Dictionary<string, object>>(jsonText);
                
                // 在字典中強制寫入或覆蓋密碼欄位
                dict["password"] = password;
                
                // 重新序列化為 JSON
                finalJsonToWrite = serializer.Serialize(dict);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"自訂 JSON 與密碼合併時發生異常：{ex.Message}", "核心合併錯誤", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // 5. 毀滅性格式化防呆確認 (二次彈出框)
            string warningMsg = $"您確定要對 PhysicalDrive{target.DriveNumber} ({target.Model}) 進行製鎖燒錄嗎？\n\n" +
                                "[警告] 此操作將會「清除該隨身碟上所有的檔案與分割區」，資料將無法找回！";
            
            var confirm1 = MessageBox.Show(warningMsg, "毀滅性操作警告 (第一次確認)", MessageBoxButtons.YesNo, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2);
            if (confirm1 != DialogResult.Yes)
            {
                lblStatus.Text = "操作已取消。";
                lblStatus.ForeColor = Color.Blue;
                return;
            }

            var confirm2 = MessageBox.Show("此動作不可逆！確定該隨身碟中無重要資料，要立即開始清除並製鎖嗎？", "最後確認", MessageBoxButtons.YesNo, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2);
            if (confirm2 != DialogResult.Yes)
            {
                lblStatus.Text = "操作已取消。";
                lblStatus.ForeColor = Color.Blue;
                return;
            }

            // 6. 開始製鎖
            this.Cursor = Cursors.WaitCursor;
            btnBurn.Enabled = false;
            btnRefresh.Enabled = false;
            cboUsbs.Enabled = false;

            try
            {
                // 第一步：清除磁碟並建立隱形分割區
                lblStatus.Text = "正在清除隨身碟並建立隱形分割區 (請勿拔除裝置)...";
                lblStatus.ForeColor = Color.DarkOrange;
                Application.DoEvents(); // 刷新 UI

                bool partitionOk = CreateHiddenPartition(target.DriveNumber);
                if (!partitionOk)
                {
                    lblStatus.Text = "建立隱形分割區失敗！";
                    lblStatus.ForeColor = Color.Red;
                    MessageBox.Show("製鎖失敗：無法建立隱藏分割區，請確認是否以管理員權限運行，或隨身碟被寫入保護。", "製鎖失敗", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                // 第二步：加密並寫入合併密碼後的 JSON 授權資料
                lblStatus.Text = "隱形分割區建立成功，正在加密寫入授權字典...";
                Application.DoEvents();

                bool writeOk = WriteHiddenData(target.DriveNumber, finalJsonToWrite, password);
                if (writeOk)
                {
                    lblStatus.Text = "燒錄成功！隨身碟已成功轉換為隱形軟體鎖。";
                    lblStatus.ForeColor = Color.Green;
                    
                    // 計算並列出 License Key 供開發者記錄
                    // 為了方便開發者，我們可以在對話框中直接列出金鑰計算公式結果
                    MessageBox.Show($"恭喜！隱形授權隨身碟 (Dongle) 燒錄完成！\n\n" +
                                    $"[金鑰對接資訊]\n" +
                                    $"硬體唯一序號：{target.SerialNumber}\n" +
                                    $"自訂製鎖密碼：{password}\n\n" +
                                    $"請依照此資訊使用 SHA-256(序號 + 密碼) 生成 License Key 寫入您的主程式驗證。", 
                                    "燒錄成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    lblStatus.Text = "加密寫入資料失敗！";
                    lblStatus.ForeColor = Color.Red;
                    MessageBox.Show("製鎖失敗：隱形分割區建立成功，但加密寫入 license.dat 失敗。", "寫入失敗", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            catch (Exception ex)
            {
                lblStatus.Text = "製鎖執行期出錯！";
                lblStatus.ForeColor = Color.Red;
                MessageBox.Show($"錯誤資訊: {ex.Message}", "執行期錯誤", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                this.Cursor = Cursors.Default;
                btnBurn.Enabled = true;
                btnRefresh.Enabled = true;
                cboUsbs.Enabled = true;
                RefreshUsbList();
            }
        }

        // 嚴格驗證 JSON 結構的輔助函式並回傳錯誤原因 (使用 .NET 內建 JavaScriptSerializer)
        private bool IsValidJson(string input, out string errorMessage)
        {
            errorMessage = "";
            if (string.IsNullOrWhiteSpace(input))
            {
                errorMessage = "JSON 輸入不可為空白。";
                return false;
            }
            try
            {
                var serializer = new System.Web.Script.Serialization.JavaScriptSerializer();
                serializer.DeserializeObject(input);
                return true;
            }
            catch (Exception ex)
            {
                errorMessage = ex.Message; // 包含詳細的錯誤原因與字元索引
                return false;
            }
        }
    }

    // USB 裝置資訊結構體 (必須與 C++ dll 共用結構體完全一致)
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct UsbDeviceInfo
    {
        public int DriveNumber; // 實體磁碟代號 (例如 1 代表 PhysicalDrive1)
        
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
        public string SerialNumber; // 晶片唯獨硬體序號
        
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
        public string Model; // 裝置型號名稱
        
        public ulong Size; // 磁碟總容量 (Bytes)
    }

    // 封裝 ComboBox 項目的類別，便於漂亮呈現隨身碟屬性
    public class UsbDeviceItem
    {
        public UsbDeviceInfo Info { get; }
        
        public UsbDeviceItem(UsbDeviceInfo info)
        {
            Info = info;
        }

        public override string ToString()
        {
            double gbSize = (double)Info.Size / (1024.0 * 1024.0 * 1024.0);
            return $"PhysicalDrive{Info.DriveNumber} | {Info.Model} | {gbSize:F2} GB";
        }
    }
}
