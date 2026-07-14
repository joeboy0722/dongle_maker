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
            this.lblUsb = new System.Windows.Forms.Label();
            this.cboUsbs = new System.Windows.Forms.ComboBox();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.lblPassword = new System.Windows.Forms.Label();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.lblJson = new System.Windows.Forms.Label();
            this.txtJsonData = new System.Windows.Forms.TextBox();
            this.lblStatusLabel = new System.Windows.Forms.Label();
            this.lblStatus = new System.Windows.Forms.Label();
            this.btnBurn = new System.Windows.Forms.Button();
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
            // lblUsb
            // 
            this.lblUsb.AutoSize = true;
            this.lblUsb.Font = new System.Drawing.Font("微軟正黑體", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.lblUsb.Location = new System.Drawing.Point(13, 46);
            this.lblUsb.Name = "lblUsb";
            this.lblUsb.Size = new System.Drawing.Size(92, 18);
            this.lblUsb.TabIndex = 1;
            this.lblUsb.Text = "選取隨身碟：";
            // 
            // cboUsbs
            // 
            this.cboUsbs.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboUsbs.Font = new System.Drawing.Font("微軟正黑體", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.cboUsbs.FormattingEnabled = true;
            this.cboUsbs.Location = new System.Drawing.Point(16, 68);
            this.cboUsbs.Name = "cboUsbs";
            this.cboUsbs.Size = new System.Drawing.Size(360, 25);
            this.cboUsbs.TabIndex = 2;
            // 
            // btnRefresh
            // 
            this.btnRefresh.Font = new System.Drawing.Font("微軟正黑體", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.btnRefresh.Location = new System.Drawing.Point(390, 66);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(110, 28);
            this.btnRefresh.TabIndex = 3;
            this.btnRefresh.Text = "重新整理";
            this.btnRefresh.UseVisualStyleBackColor = true;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // lblPassword
            // 
            this.lblPassword.AutoSize = true;
            this.lblPassword.Font = new System.Drawing.Font("微軟正黑體", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.lblPassword.Location = new System.Drawing.Point(13, 105);
            this.lblPassword.Name = "lblPassword";
            this.lblPassword.Size = new System.Drawing.Size(185, 18);
            this.lblPassword.TabIndex = 4;
            this.lblPassword.Text = "自訂製鎖密碼 (Password)：";
            // 
            // txtPassword
            // 
            this.txtPassword.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtPassword.Location = new System.Drawing.Point(16, 125);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.Size = new System.Drawing.Size(484, 23);
            this.txtPassword.TabIndex = 5;
            // 
            // lblJson
            // 
            this.lblJson.AutoSize = true;
            this.lblJson.Font = new System.Drawing.Font("微軟正黑體", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.lblJson.Location = new System.Drawing.Point(13, 165);
            this.lblJson.Name = "lblJson";
            this.lblJson.Size = new System.Drawing.Size(202, 18);
            this.lblJson.TabIndex = 6;
            this.lblJson.Text = "授權資料字典設定 (JSON 格式)：";
            // 
            // txtJsonData
            // 
            this.txtJsonData.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtJsonData.Location = new System.Drawing.Point(16, 187);
            this.txtJsonData.Multiline = true;
            this.txtJsonData.Name = "txtJsonData";
            this.txtJsonData.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtJsonData.Size = new System.Drawing.Size(484, 180);
            this.txtJsonData.TabIndex = 7;
            // 
            // lblStatusLabel
            // 
            this.lblStatusLabel.AutoSize = true;
            this.lblStatusLabel.Font = new System.Drawing.Font("微軟正黑體", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.lblStatusLabel.Location = new System.Drawing.Point(13, 385);
            this.lblStatusLabel.Name = "lblStatusLabel";
            this.lblStatusLabel.Size = new System.Drawing.Size(78, 18);
            this.lblStatusLabel.TabIndex = 8;
            this.lblStatusLabel.Text = "目前狀態：";
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.Font = new System.Drawing.Font("微軟正黑體", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.lblStatus.ForeColor = System.Drawing.Color.Blue;
            this.lblStatus.Location = new System.Drawing.Point(90, 385);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(78, 18);
            this.lblStatus.TabIndex = 9;
            this.lblStatus.Text = "等待操作...";
            // 
            // btnBurn
            // 
            this.btnBurn.BackColor = System.Drawing.Color.DarkRed;
            this.btnBurn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnBurn.Font = new System.Drawing.Font("微軟正黑體", 11F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.btnBurn.ForeColor = System.Drawing.Color.White;
            this.btnBurn.Location = new System.Drawing.Point(16, 420);
            this.btnBurn.Name = "btnBurn";
            this.btnBurn.Size = new System.Drawing.Size(484, 45);
            this.btnBurn.TabIndex = 10;
            this.btnBurn.Text = "抹除磁碟並寫入隱形授權鎖";
            this.btnBurn.UseVisualStyleBackColor = false;
            this.btnBurn.Click += new System.EventHandler(this.btnBurn_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(517, 481);
            this.Controls.Add(this.btnBurn);
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.lblStatusLabel);
            this.Controls.Add(this.txtJsonData);
            this.Controls.Add(this.lblJson);
            this.Controls.Add(this.txtPassword);
            this.Controls.Add(this.lblPassword);
            this.Controls.Add(this.btnRefresh);
            this.Controls.Add(this.cboUsbs);
            this.Controls.Add(this.lblUsb);
            this.Controls.Add(this.lblTitle);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "隱形 USB 軟體鎖燒錄工具";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lblTitle;
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
    }
}
