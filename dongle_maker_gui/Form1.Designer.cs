namespace dongle_maker_gui
{
    partial class Form1
    {
        /// <summary>
        /// 設計工具所需的變數。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清除任何使用中的資源。
        /// </summary>
        /// <param name="disposing">如果應該處置受控資源則為 true，否則為 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 設計工具產生的程式碼

        /// <summary>
        /// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改
        /// 這個方法的內容。
        /// </summary>
        private void InitializeComponent()
        {
            this.lblTitle = new System.Windows.Forms.Label();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPageMaker = new System.Windows.Forms.TabPage();
            this.btnBurn = new System.Windows.Forms.Button();
            this.lblStatus = new System.Windows.Forms.Label();
            this.lblStatusLabel = new System.Windows.Forms.Label();
            this.txtJsonData = new System.Windows.Forms.TextBox();
            this.lblJson = new System.Windows.Forms.Label();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.lblPassword = new System.Windows.Forms.Label();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.cboUsbs = new System.Windows.Forms.ComboBox();
            this.lblUsb = new System.Windows.Forms.Label();
            this.tabPageReader = new System.Windows.Forms.TabPage();
            this.grpLicenseDetails = new System.Windows.Forms.GroupBox();
            this.txtLicenseData = new System.Windows.Forms.TextBox();
            this.lblLicenseData = new System.Windows.Forms.Label();
            this.txtSerial = new System.Windows.Forms.TextBox();
            this.lblSerial = new System.Windows.Forms.Label();
            this.lblReaderStatus = new System.Windows.Forms.Label();
            this.lblReaderStatusLabel = new System.Windows.Forms.Label();
            this.btnVerify = new System.Windows.Forms.Button();
            this.txtReaderPassword = new System.Windows.Forms.TextBox();
            this.lblReaderPassword = new System.Windows.Forms.Label();
            this.btnReaderRefresh = new System.Windows.Forms.Button();
            this.cboReaderUsbs = new System.Windows.Forms.ComboBox();
            this.lblReaderUsb = new System.Windows.Forms.Label();
            this.tabControl1.SuspendLayout();
            this.tabPageMaker.SuspendLayout();
            this.tabPageReader.SuspendLayout();
            this.grpLicenseDetails.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblTitle
            // 
            this.lblTitle.AutoSize = true;
            this.lblTitle.Font = new System.Drawing.Font("微軟正黑體", 14F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.lblTitle.Location = new System.Drawing.Point(12, 9);
            this.lblTitle.Name = "lblTitle";
            this.lblTitle.Size = new System.Drawing.Size(325, 24);
            this.lblTitle.TabIndex = 0;
            this.lblTitle.Text = "隱形 USB 軟體鎖 (Dongle) 燒錄工具";
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPageMaker);
            this.tabControl1.Controls.Add(this.tabPageReader);
            this.tabControl1.Font = new System.Drawing.Font("微軟正黑體", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.tabControl1.Location = new System.Drawing.Point(12, 45);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(493, 465);
            this.tabControl1.TabIndex = 1;
            // 
            // tabPageMaker
            // 
            this.tabPageMaker.Controls.Add(this.btnBurn);
            this.tabPageMaker.Controls.Add(this.lblStatus);
            this.tabPageMaker.Controls.Add(this.lblStatusLabel);
            this.tabPageMaker.Controls.Add(this.txtJsonData);
            this.tabPageMaker.Controls.Add(this.lblJson);
            this.tabPageMaker.Controls.Add(this.txtPassword);
            this.tabPageMaker.Controls.Add(this.lblPassword);
            this.tabPageMaker.Controls.Add(this.btnRefresh);
            this.tabPageMaker.Controls.Add(this.cboUsbs);
            this.tabPageMaker.Controls.Add(this.lblUsb);
            this.tabPageMaker.Location = new System.Drawing.Point(4, 26);
            this.tabPageMaker.Name = "tabPageMaker";
            this.tabPageMaker.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageMaker.Size = new System.Drawing.Size(485, 435);
            this.tabPageMaker.TabIndex = 0;
            this.tabPageMaker.Text = "製作軟體鎖 (Maker)";
            this.tabPageMaker.UseVisualStyleBackColor = true;
            // 
            // btnBurn
            // 
            this.btnBurn.BackColor = System.Drawing.Color.DarkRed;
            this.btnBurn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnBurn.Font = new System.Drawing.Font("微軟正黑體", 11F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.btnBurn.ForeColor = System.Drawing.Color.White;
            this.btnBurn.Location = new System.Drawing.Point(15, 370);
            this.btnBurn.Name = "btnBurn";
            this.btnBurn.Size = new System.Drawing.Size(450, 45);
            this.btnBurn.TabIndex = 9;
            this.btnBurn.Text = "抹除磁碟並寫入隱形授權鎖";
            this.btnBurn.UseVisualStyleBackColor = false;
            this.btnBurn.Click += new System.EventHandler(this.btnBurn_Click);
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.ForeColor = System.Drawing.Color.Blue;
            this.lblStatus.Location = new System.Drawing.Point(90, 335);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(78, 18);
            this.lblStatus.TabIndex = 8;
            this.lblStatus.Text = "等待操作...";
            // 
            // lblStatusLabel
            // 
            this.lblStatusLabel.AutoSize = true;
            this.lblStatusLabel.Font = new System.Drawing.Font("微軟正黑體", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.lblStatusLabel.Location = new System.Drawing.Point(15, 335);
            this.lblStatusLabel.Name = "lblStatusLabel";
            this.lblStatusLabel.Size = new System.Drawing.Size(78, 18);
            this.lblStatusLabel.TabIndex = 7;
            this.lblStatusLabel.Text = "目前狀態：";
            // 
            // txtJsonData
            // 
            this.txtJsonData.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtJsonData.Location = new System.Drawing.Point(15, 159);
            this.txtJsonData.Multiline = true;
            this.txtJsonData.Name = "txtJsonData";
            this.txtJsonData.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtJsonData.Size = new System.Drawing.Size(450, 160);
            this.txtJsonData.TabIndex = 6;
            // 
            // lblJson
            // 
            this.lblJson.AutoSize = true;
            this.lblJson.Location = new System.Drawing.Point(12, 137);
            this.lblJson.Name = "lblJson";
            this.lblJson.Size = new System.Drawing.Size(202, 18);
            this.lblJson.TabIndex = 5;
            this.lblJson.Text = "授權資料字典設定 (JSON 格式)：";
            // 
            // txtPassword
            // 
            this.txtPassword.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtPassword.Location = new System.Drawing.Point(15, 97);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.Size = new System.Drawing.Size(450, 23);
            this.txtPassword.TabIndex = 4;
            // 
            // lblPassword
            // 
            this.lblPassword.AutoSize = true;
            this.lblPassword.Location = new System.Drawing.Point(12, 75);
            this.lblPassword.Name = "lblPassword";
            this.lblPassword.Size = new System.Drawing.Size(185, 18);
            this.lblPassword.TabIndex = 3;
            this.lblPassword.Text = "自訂製鎖密碼 (Password)：";
            // 
            // btnRefresh
            // 
            this.btnRefresh.Location = new System.Drawing.Point(355, 35);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(110, 28);
            this.btnRefresh.TabIndex = 2;
            this.btnRefresh.Text = "重新整理";
            this.btnRefresh.UseVisualStyleBackColor = true;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // cboUsbs
            // 
            this.cboUsbs.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboUsbs.FormattingEnabled = true;
            this.cboUsbs.Location = new System.Drawing.Point(15, 37);
            this.cboUsbs.Name = "cboUsbs";
            this.cboUsbs.Size = new System.Drawing.Size(330, 25);
            this.cboUsbs.TabIndex = 1;
            // 
            // lblUsb
            // 
            this.lblUsb.AutoSize = true;
            this.lblUsb.Location = new System.Drawing.Point(12, 15);
            this.lblUsb.Name = "lblUsb";
            this.lblUsb.Size = new System.Drawing.Size(92, 18);
            this.lblUsb.TabIndex = 0;
            this.lblUsb.Text = "選取隨身碟：";
            // 
            // tabPageReader
            // 
            this.tabPageReader.Controls.Add(this.grpLicenseDetails);
            this.tabPageReader.Controls.Add(this.lblReaderStatus);
            this.tabPageReader.Controls.Add(this.lblReaderStatusLabel);
            this.tabPageReader.Controls.Add(this.btnVerify);
            this.tabPageReader.Controls.Add(this.txtReaderPassword);
            this.tabPageReader.Controls.Add(this.lblReaderPassword);
            this.tabPageReader.Controls.Add(this.btnReaderRefresh);
            this.tabPageReader.Controls.Add(this.cboReaderUsbs);
            this.tabPageReader.Controls.Add(this.lblReaderUsb);
            this.tabPageReader.Location = new System.Drawing.Point(4, 26);
            this.tabPageReader.Name = "tabPageReader";
            this.tabPageReader.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageReader.Size = new System.Drawing.Size(485, 435);
            this.tabPageReader.TabIndex = 1;
            this.tabPageReader.Text = "讀取與驗證 (Reader Demo)";
            this.tabPageReader.UseVisualStyleBackColor = true;
            // 
            // grpLicenseDetails
            // 
            this.grpLicenseDetails.Controls.Add(this.txtLicenseData);
            this.grpLicenseDetails.Controls.Add(this.lblLicenseData);
            this.grpLicenseDetails.Controls.Add(this.txtSerial);
            this.grpLicenseDetails.Controls.Add(this.lblSerial);
            this.grpLicenseDetails.Location = new System.Drawing.Point(15, 225);
            this.grpLicenseDetails.Name = "grpLicenseDetails";
            this.grpLicenseDetails.Size = new System.Drawing.Size(450, 195);
            this.grpLicenseDetails.TabIndex = 8;
            this.grpLicenseDetails.TabStop = false;
            this.grpLicenseDetails.Text = "授權詳細資訊";
            // 
            // txtLicenseData
            // 
            this.txtLicenseData.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.txtLicenseData.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtLicenseData.Location = new System.Drawing.Point(15, 107);
            this.txtLicenseData.Multiline = true;
            this.txtLicenseData.Name = "txtLicenseData";
            this.txtLicenseData.ReadOnly = true;
            this.txtLicenseData.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtLicenseData.Size = new System.Drawing.Size(420, 75);
            this.txtLicenseData.TabIndex = 3;
            // 
            // lblLicenseData
            // 
            this.lblLicenseData.AutoSize = true;
            this.lblLicenseData.Location = new System.Drawing.Point(12, 85);
            this.lblLicenseData.Name = "lblLicenseData";
            this.lblLicenseData.Size = new System.Drawing.Size(160, 18);
            this.lblLicenseData.TabIndex = 2;
            this.lblLicenseData.Text = "解密後授權資料 (JSON)：";
            // 
            // txtSerial
            // 
            this.txtSerial.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.txtSerial.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtSerial.Location = new System.Drawing.Point(15, 47);
            this.txtSerial.Name = "txtSerial";
            this.txtSerial.ReadOnly = true;
            this.txtSerial.Size = new System.Drawing.Size(420, 23);
            this.txtSerial.TabIndex = 1;
            // 
            // lblSerial
            // 
            this.lblSerial.AutoSize = true;
            this.lblSerial.Location = new System.Drawing.Point(12, 25);
            this.lblSerial.Name = "lblSerial";
            this.lblSerial.Size = new System.Drawing.Size(134, 18);
            this.lblSerial.TabIndex = 0;
            this.lblSerial.Text = "USB 晶片硬體序號：";
            // 
            // lblReaderStatus
            // 
            this.lblReaderStatus.AutoSize = true;
            this.lblReaderStatus.ForeColor = System.Drawing.Color.Blue;
            this.lblReaderStatus.Location = new System.Drawing.Point(90, 195);
            this.lblReaderStatus.Name = "lblReaderStatus";
            this.lblReaderStatus.Size = new System.Drawing.Size(64, 18);
            this.lblReaderStatus.TabIndex = 7;
            this.lblReaderStatus.Text = "等待驗證...";
            // 
            // lblReaderStatusLabel
            // 
            this.lblReaderStatusLabel.AutoSize = true;
            this.lblReaderStatusLabel.Font = new System.Drawing.Font("微軟正黑體", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.lblReaderStatusLabel.Location = new System.Drawing.Point(15, 195);
            this.lblReaderStatusLabel.Name = "lblReaderStatusLabel";
            this.lblReaderStatusLabel.Size = new System.Drawing.Size(78, 18);
            this.lblReaderStatusLabel.TabIndex = 6;
            this.lblReaderStatusLabel.Text = "驗證狀態：";
            // 
            // btnVerify
            // 
            this.btnVerify.BackColor = System.Drawing.Color.DarkGreen;
            this.btnVerify.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnVerify.Font = new System.Drawing.Font("微軟正黑體", 11F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.btnVerify.ForeColor = System.Drawing.Color.White;
            this.btnVerify.Location = new System.Drawing.Point(15, 137);
            this.btnVerify.Name = "btnVerify";
            this.btnVerify.Size = new System.Drawing.Size(450, 45);
            this.btnVerify.TabIndex = 5;
            this.btnVerify.Text = "讀取並驗證軟體鎖 (Demo)";
            this.btnVerify.UseVisualStyleBackColor = false;
            this.btnVerify.Click += new System.EventHandler(this.btnVerify_Click);
            // 
            // txtReaderPassword
            // 
            this.txtReaderPassword.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtReaderPassword.Location = new System.Drawing.Point(15, 97);
            this.txtReaderPassword.Name = "txtReaderPassword";
            this.txtReaderPassword.Size = new System.Drawing.Size(450, 23);
            this.txtReaderPassword.TabIndex = 4;
            // 
            // lblReaderPassword
            // 
            this.lblReaderPassword.AutoSize = true;
            this.lblReaderPassword.Location = new System.Drawing.Point(12, 75);
            this.lblReaderPassword.Name = "lblReaderPassword";
            this.lblReaderPassword.Size = new System.Drawing.Size(120, 18);
            this.lblReaderPassword.TabIndex = 3;
            this.lblReaderPassword.Text = "請輸入解密密碼：";
            // 
            // btnReaderRefresh
            // 
            this.btnReaderRefresh.Location = new System.Drawing.Point(355, 35);
            this.btnReaderRefresh.Name = "btnReaderRefresh";
            this.btnReaderRefresh.Size = new System.Drawing.Size(110, 28);
            this.btnReaderRefresh.TabIndex = 2;
            this.btnReaderRefresh.Text = "重新整理";
            this.btnReaderRefresh.UseVisualStyleBackColor = true;
            this.btnReaderRefresh.Click += new System.EventHandler(this.btnReaderRefresh_Click);
            // 
            // cboReaderUsbs
            // 
            this.cboReaderUsbs.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboReaderUsbs.FormattingEnabled = true;
            this.cboReaderUsbs.Location = new System.Drawing.Point(15, 37);
            this.cboReaderUsbs.Name = "cboReaderUsbs";
            this.cboReaderUsbs.Size = new System.Drawing.Size(330, 25);
            this.cboReaderUsbs.TabIndex = 1;
            // 
            // lblReaderUsb
            // 
            this.lblReaderUsb.AutoSize = true;
            this.lblReaderUsb.Location = new System.Drawing.Point(12, 15);
            this.lblReaderUsb.Name = "lblReaderUsb";
            this.lblReaderUsb.Size = new System.Drawing.Size(92, 18);
            this.lblReaderUsb.TabIndex = 0;
            this.lblReaderUsb.Text = "選取隨身碟：";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(517, 521);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.lblTitle);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "USB 隱形授權鎖燒錄與驗證工具 (開發/DEMO)";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPageMaker.ResumeLayout(false);
            this.tabPageMaker.PerformLayout();
            this.tabPageReader.ResumeLayout(false);
            this.tabPageReader.PerformLayout();
            this.grpLicenseDetails.ResumeLayout(false);
            this.grpLicenseDetails.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lblTitle;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPageMaker;
        private System.Windows.Forms.TabPage tabPageReader;
        
        // Maker Controls
        private System.Windows.Forms.Label lblUsb;
        private System.Windows.Forms.ComboBox cboUsbs;
        private System.Windows.Forms.Button btnRefresh;
        private System.Windows.Forms.Label lblPassword;
        private System.Windows.Forms.TextBox txtPassword;
        private System.Windows.Forms.Label lblJson;
        private System.Windows.Forms.TextBox txtJsonData;
        private System.Windows.Forms.Label lblStatusLabel;
        private System.Windows.Forms.Label lblStatus;
        private System.Windows.Forms.Button btnBurn;

        // Reader Controls
        private System.Windows.Forms.Label lblReaderUsb;
        private System.Windows.Forms.ComboBox cboReaderUsbs;
        private System.Windows.Forms.Button btnReaderRefresh;
        private System.Windows.Forms.Label lblReaderPassword;
        private System.Windows.Forms.TextBox txtReaderPassword;
        private System.Windows.Forms.Button btnVerify;
        private System.Windows.Forms.Label lblReaderStatusLabel;
        private System.Windows.Forms.Label lblReaderStatus;
        private System.Windows.Forms.GroupBox grpLicenseDetails;
        private System.Windows.Forms.Label lblSerial;
        private System.Windows.Forms.TextBox txtSerial;
        private System.Windows.Forms.Label lblLicenseData;
        private System.Windows.Forms.TextBox txtLicenseData;
    }
}
