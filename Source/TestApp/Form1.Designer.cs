namespace TestApp
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.TBConnectStatus = new System.Windows.Forms.TextBox();
            this.LabAPI = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.MyAcceptButton = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.tabMemoryPage = new System.Windows.Forms.TabPage();
            this.label6 = new System.Windows.Forms.Label();
            this.TBMemAddress = new System.Windows.Forms.TextBox();
            this.MemTextBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.H = new System.Windows.Forms.Label();
            this.ButLineHigher = new System.Windows.Forms.Button();
            this.ButPageHigher = new System.Windows.Forms.Button();
            this.ButLineLower = new System.Windows.Forms.Button();
            this.ButPageLower = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabMenuPage = new System.Windows.Forms.TabPage();
            this.LMenuTitle = new System.Windows.Forms.Label();
            this.EscapeButton = new System.Windows.Forms.Button();
            this.MenuListBox = new System.Windows.Forms.ListBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.nudCurrentChannel = new System.Windows.Forms.NumericUpDown();
            this.bDisconnect = new System.Windows.Forms.Button();
            this.rbSerial = new System.Windows.Forms.RadioButton();
            this.rbUSB = new System.Windows.Forms.RadioButton();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.rbSelectSerial = new System.Windows.Forms.RadioButton();
            this.rbSelectUSB = new System.Windows.Forms.RadioButton();
            this.label1 = new System.Windows.Forms.Label();
            this.nudIndex = new System.Windows.Forms.NumericUpDown();
            this.bConnect = new System.Windows.Forms.Button();
            this.tbMilliseconds = new System.Windows.Forms.TextBox();
            this.labMsec = new System.Windows.Forms.Label();
            this.rtbMessageBuff = new System.Windows.Forms.RichTextBox();
            this.label56 = new System.Windows.Forms.Label();
            this.cbMessBuffAutoScroll = new System.Windows.Forms.CheckBox();
            this.bMessageBufferClear = new System.Windows.Forms.Button();
            this.ProgBarLabel = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.bMessWriteFile = new System.Windows.Forms.Button();
            this.tabMemoryPage.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabMenuPage.SuspendLayout();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudCurrentChannel)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudIndex)).BeginInit();
            this.SuspendLayout();
            // 
            // TBConnectStatus
            // 
            this.TBConnectStatus.Location = new System.Drawing.Point(318, 514);
            this.TBConnectStatus.Name = "TBConnectStatus";
            this.TBConnectStatus.Size = new System.Drawing.Size(55, 20);
            this.TBConnectStatus.TabIndex = 1;
            // 
            // LabAPI
            // 
            this.LabAPI.AutoSize = true;
            this.LabAPI.Location = new System.Drawing.Point(324, 539);
            this.LabAPI.Name = "LabAPI";
            this.LabAPI.Size = new System.Drawing.Size(49, 13);
            this.LabAPI.TabIndex = 4;
            this.LabAPI.Text = "Link Stat";
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 10;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // MyAcceptButton
            // 
            this.MyAcceptButton.Location = new System.Drawing.Point(653, 530);
            this.MyAcceptButton.Name = "MyAcceptButton";
            this.MyAcceptButton.Size = new System.Drawing.Size(66, 30);
            this.MyAcceptButton.TabIndex = 5;
            this.MyAcceptButton.Text = "Accept";
            this.MyAcceptButton.UseVisualStyleBackColor = true;
            this.MyAcceptButton.Click += new System.EventHandler(this.MyAcceptButton_Click);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(468, 493);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(262, 24);
            this.progressBar1.TabIndex = 8;
            this.progressBar1.Visible = false;
            // 
            // tabMemoryPage
            // 
            this.tabMemoryPage.BackColor = System.Drawing.Color.Moccasin;
            this.tabMemoryPage.Controls.Add(this.label6);
            this.tabMemoryPage.Controls.Add(this.TBMemAddress);
            this.tabMemoryPage.Controls.Add(this.MemTextBox);
            this.tabMemoryPage.Controls.Add(this.label5);
            this.tabMemoryPage.Controls.Add(this.H);
            this.tabMemoryPage.Controls.Add(this.ButLineHigher);
            this.tabMemoryPage.Controls.Add(this.ButPageHigher);
            this.tabMemoryPage.Controls.Add(this.ButLineLower);
            this.tabMemoryPage.Controls.Add(this.ButPageLower);
            this.tabMemoryPage.Location = new System.Drawing.Point(4, 22);
            this.tabMemoryPage.Name = "tabMemoryPage";
            this.tabMemoryPage.Padding = new System.Windows.Forms.Padding(3);
            this.tabMemoryPage.Size = new System.Drawing.Size(918, 464);
            this.tabMemoryPage.TabIndex = 2;
            this.tabMemoryPage.Text = "Memory";
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(380, 320);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(86, 17);
            this.label6.TabIndex = 9;
            this.label6.Text = "Address";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // TBMemAddress
            // 
            this.TBMemAddress.Location = new System.Drawing.Point(397, 335);
            this.TBMemAddress.MaxLength = 10;
            this.TBMemAddress.Name = "TBMemAddress";
            this.TBMemAddress.Size = new System.Drawing.Size(69, 20);
            this.TBMemAddress.TabIndex = 8;
            this.TBMemAddress.Text = "0X00000000";
            this.TBMemAddress.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.TBMemAddress.WordWrap = false;
            this.TBMemAddress.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TBMemAddress_KeyDown);
            this.TBMemAddress.Leave += new System.EventHandler(this.TBMemAddress_Leave);
            // 
            // MemTextBox
            // 
            this.MemTextBox.BackColor = System.Drawing.Color.Bisque;
            this.MemTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MemTextBox.Location = new System.Drawing.Point(6, 28);
            this.MemTextBox.Multiline = true;
            this.MemTextBox.Name = "MemTextBox";
            this.MemTextBox.Size = new System.Drawing.Size(602, 222);
            this.MemTextBox.TabIndex = 0;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(463, 350);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(38, 13);
            this.label5.TabIndex = 7;
            this.label5.Text = "Higher";
            // 
            // H
            // 
            this.H.AutoSize = true;
            this.H.Location = new System.Drawing.Point(465, 320);
            this.H.Name = "H";
            this.H.Size = new System.Drawing.Size(36, 13);
            this.H.TabIndex = 6;
            this.H.Text = "Lower";
            // 
            // ButLineHigher
            // 
            this.ButLineHigher.Location = new System.Drawing.Point(553, 344);
            this.ButLineHigher.Name = "ButLineHigher";
            this.ButLineHigher.Size = new System.Drawing.Size(38, 24);
            this.ButLineHigher.TabIndex = 5;
            this.ButLineHigher.Text = "Line";
            this.ButLineHigher.UseVisualStyleBackColor = true;
            this.ButLineHigher.Click += new System.EventHandler(this.ButLineHigher_Click);
            // 
            // ButPageHigher
            // 
            this.ButPageHigher.Location = new System.Drawing.Point(507, 344);
            this.ButPageHigher.Name = "ButPageHigher";
            this.ButPageHigher.Size = new System.Drawing.Size(40, 24);
            this.ButPageHigher.TabIndex = 4;
            this.ButPageHigher.Text = "Page";
            this.ButPageHigher.UseVisualStyleBackColor = true;
            this.ButPageHigher.Click += new System.EventHandler(this.ButPageHigher_Click);
            // 
            // ButLineLower
            // 
            this.ButLineLower.Location = new System.Drawing.Point(553, 314);
            this.ButLineLower.Name = "ButLineLower";
            this.ButLineLower.Size = new System.Drawing.Size(38, 23);
            this.ButLineLower.TabIndex = 3;
            this.ButLineLower.Text = "Line";
            this.ButLineLower.UseVisualStyleBackColor = true;
            this.ButLineLower.Click += new System.EventHandler(this.ButLineLower_Click);
            // 
            // ButPageLower
            // 
            this.ButPageLower.Location = new System.Drawing.Point(507, 314);
            this.ButPageLower.Name = "ButPageLower";
            this.ButPageLower.Size = new System.Drawing.Size(40, 24);
            this.ButPageLower.TabIndex = 2;
            this.ButPageLower.Text = "Page";
            this.ButPageLower.UseVisualStyleBackColor = true;
            this.ButPageLower.Click += new System.EventHandler(this.ButPageLower_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabMenuPage);
            this.tabControl1.Controls.Add(this.tabMemoryPage);
            this.tabControl1.Location = new System.Drawing.Point(2, -1);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(926, 490);
            this.tabControl1.TabIndex = 0;
            this.tabControl1.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
            // 
            // tabMenuPage
            // 
            this.tabMenuPage.BackColor = System.Drawing.Color.Transparent;
            this.tabMenuPage.BackgroundImage = global::TestApp.Properties.Resources.IMG_1218;
            this.tabMenuPage.Controls.Add(this.LMenuTitle);
            this.tabMenuPage.Controls.Add(this.EscapeButton);
            this.tabMenuPage.Controls.Add(this.MenuListBox);
            this.tabMenuPage.Location = new System.Drawing.Point(4, 22);
            this.tabMenuPage.Name = "tabMenuPage";
            this.tabMenuPage.Padding = new System.Windows.Forms.Padding(3);
            this.tabMenuPage.Size = new System.Drawing.Size(918, 464);
            this.tabMenuPage.TabIndex = 0;
            this.tabMenuPage.Text = "Menu";
            // 
            // LMenuTitle
            // 
            this.LMenuTitle.BackColor = System.Drawing.SystemColors.Menu;
            this.LMenuTitle.Location = new System.Drawing.Point(-25, -66);
            this.LMenuTitle.Name = "LMenuTitle";
            this.LMenuTitle.Size = new System.Drawing.Size(437, 15);
            this.LMenuTitle.TabIndex = 9;
            this.LMenuTitle.Text = "Menu Title";
            this.LMenuTitle.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // EscapeButton
            // 
            this.EscapeButton.Location = new System.Drawing.Point(707, 433);
            this.EscapeButton.Name = "EscapeButton";
            this.EscapeButton.Size = new System.Drawing.Size(66, 28);
            this.EscapeButton.TabIndex = 1;
            this.EscapeButton.Text = "Escape";
            this.EscapeButton.UseVisualStyleBackColor = true;
            this.EscapeButton.Click += new System.EventHandler(this.EscapeButton_Click);
            // 
            // MenuListBox
            // 
            this.MenuListBox.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.MenuListBox.BackColor = System.Drawing.SystemColors.Info;
            this.MenuListBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MenuListBox.FormattingEnabled = true;
            this.MenuListBox.ItemHeight = 16;
            this.MenuListBox.Location = new System.Drawing.Point(182, 49);
            this.MenuListBox.Name = "MenuListBox";
            this.MenuListBox.Size = new System.Drawing.Size(440, 372);
            this.MenuListBox.TabIndex = 0;
            this.MenuListBox.SelectedIndexChanged += new System.EventHandler(this.MenuListBox_SelectedIndexChanged);
            this.MenuListBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.MenuListBox_KeyDown);
            this.MenuListBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.MenuListBox_KeyPress);
            this.MenuListBox.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.MenuListBox_MouseDoubleClick);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.nudCurrentChannel);
            this.groupBox1.Controls.Add(this.bDisconnect);
            this.groupBox1.Controls.Add(this.rbSerial);
            this.groupBox1.Controls.Add(this.rbUSB);
            this.groupBox1.Location = new System.Drawing.Point(0, 489);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(149, 70);
            this.groupBox1.TabIndex = 9;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Current Channel";
            // 
            // nudCurrentChannel
            // 
            this.nudCurrentChannel.Location = new System.Drawing.Point(0, 0);
            this.nudCurrentChannel.Name = "nudCurrentChannel";
            this.nudCurrentChannel.Size = new System.Drawing.Size(120, 20);
            this.nudCurrentChannel.TabIndex = 0;
            // 
            // bDisconnect
            // 
            this.bDisconnect.Location = new System.Drawing.Point(68, 41);
            this.bDisconnect.Name = "bDisconnect";
            this.bDisconnect.Size = new System.Drawing.Size(74, 23);
            this.bDisconnect.TabIndex = 3;
            this.bDisconnect.Text = "Disconnect";
            this.bDisconnect.UseVisualStyleBackColor = true;
            this.bDisconnect.Click += new System.EventHandler(this.bDisconnect_Click);
            // 
            // rbSerial
            // 
            this.rbSerial.AutoCheck = false;
            this.rbSerial.AutoSize = true;
            this.rbSerial.Location = new System.Drawing.Point(5, 39);
            this.rbSerial.Name = "rbSerial";
            this.rbSerial.Size = new System.Drawing.Size(51, 17);
            this.rbSerial.TabIndex = 1;
            this.rbSerial.TabStop = true;
            this.rbSerial.Text = "Serial";
            this.rbSerial.UseVisualStyleBackColor = true;
            this.rbSerial.CheckedChanged += new System.EventHandler(this.rbSerial_CheckedChanged);
            // 
            // rbUSB
            // 
            this.rbUSB.AutoCheck = false;
            this.rbUSB.AutoSize = true;
            this.rbUSB.Location = new System.Drawing.Point(6, 19);
            this.rbUSB.Name = "rbUSB";
            this.rbUSB.Size = new System.Drawing.Size(47, 17);
            this.rbUSB.TabIndex = 0;
            this.rbUSB.TabStop = true;
            this.rbUSB.Text = "USB";
            this.rbUSB.UseVisualStyleBackColor = true;
            this.rbUSB.CheckedChanged += new System.EventHandler(this.rbUSB_CheckedChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.rbSelectSerial);
            this.groupBox2.Controls.Add(this.rbSelectUSB);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.nudIndex);
            this.groupBox2.Controls.Add(this.bConnect);
            this.groupBox2.Location = new System.Drawing.Point(162, 490);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(147, 66);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Connect";
            this.groupBox2.Enter += new System.EventHandler(this.groupBox2_Enter);
            // 
            // rbSelectSerial
            // 
            this.rbSelectSerial.AutoSize = true;
            this.rbSelectSerial.Location = new System.Drawing.Point(6, 43);
            this.rbSelectSerial.Name = "rbSelectSerial";
            this.rbSelectSerial.Size = new System.Drawing.Size(51, 17);
            this.rbSelectSerial.TabIndex = 4;
            this.rbSelectSerial.TabStop = true;
            this.rbSelectSerial.Text = "Serial";
            this.rbSelectSerial.UseVisualStyleBackColor = true;
            this.rbSelectSerial.CheckedChanged += new System.EventHandler(this.rbSelectSerial_CheckedChanged);
            // 
            // rbSelectUSB
            // 
            this.rbSelectUSB.AutoSize = true;
            this.rbSelectUSB.Location = new System.Drawing.Point(6, 16);
            this.rbSelectUSB.Name = "rbSelectUSB";
            this.rbSelectUSB.Size = new System.Drawing.Size(47, 17);
            this.rbSelectUSB.TabIndex = 3;
            this.rbSelectUSB.TabStop = true;
            this.rbSelectUSB.Text = "USB";
            this.rbSelectUSB.UseVisualStyleBackColor = true;
            this.rbSelectUSB.CheckedChanged += new System.EventHandler(this.rbSelectUSB_CheckedChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(54, 17);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(33, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Index";
            // 
            // nudIndex
            // 
            this.nudIndex.Location = new System.Drawing.Point(55, 31);
            this.nudIndex.Maximum = new decimal(new int[] {
            30,
            0,
            0,
            0});
            this.nudIndex.Name = "nudIndex";
            this.nudIndex.Size = new System.Drawing.Size(33, 20);
            this.nudIndex.TabIndex = 1;
            this.nudIndex.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // bConnect
            // 
            this.bConnect.Location = new System.Drawing.Point(90, 9);
            this.bConnect.Name = "bConnect";
            this.bConnect.Size = new System.Drawing.Size(55, 27);
            this.bConnect.TabIndex = 0;
            this.bConnect.Text = "Connect";
            this.bConnect.UseVisualStyleBackColor = true;
            this.bConnect.Click += new System.EventHandler(this.bConnect_Click);
            // 
            // tbMilliseconds
            // 
            this.tbMilliseconds.Location = new System.Drawing.Point(743, 512);
            this.tbMilliseconds.Name = "tbMilliseconds";
            this.tbMilliseconds.Size = new System.Drawing.Size(79, 20);
            this.tbMilliseconds.TabIndex = 11;
            // 
            // labMsec
            // 
            this.labMsec.AutoSize = true;
            this.labMsec.Location = new System.Drawing.Point(751, 495);
            this.labMsec.Name = "labMsec";
            this.labMsec.Size = new System.Drawing.Size(64, 13);
            this.labMsec.TabIndex = 12;
            this.labMsec.Text = "Milliseconds";
            // 
            // rtbMessageBuff
            // 
            this.rtbMessageBuff.Location = new System.Drawing.Point(785, -1);
            this.rtbMessageBuff.Name = "rtbMessageBuff";
            this.rtbMessageBuff.Size = new System.Drawing.Size(451, 490);
            this.rtbMessageBuff.TabIndex = 13;
            this.rtbMessageBuff.Text = "";
            // 
            // label56
            // 
            this.label56.AutoSize = true;
            this.label56.Location = new System.Drawing.Point(934, 491);
            this.label56.Name = "label56";
            this.label56.Size = new System.Drawing.Size(96, 13);
            this.label56.TabIndex = 14;
            this.label56.Text = "Slave Trace Buffer";
            // 
            // cbMessBuffAutoScroll
            // 
            this.cbMessBuffAutoScroll.AutoSize = true;
            this.cbMessBuffAutoScroll.Location = new System.Drawing.Point(937, 507);
            this.cbMessBuffAutoScroll.Name = "cbMessBuffAutoScroll";
            this.cbMessBuffAutoScroll.Size = new System.Drawing.Size(77, 17);
            this.cbMessBuffAutoScroll.TabIndex = 15;
            this.cbMessBuffAutoScroll.Text = "Auto Scroll";
            this.cbMessBuffAutoScroll.UseVisualStyleBackColor = true;
            // 
            // bMessageBufferClear
            // 
            this.bMessageBufferClear.Location = new System.Drawing.Point(1056, 493);
            this.bMessageBufferClear.Name = "bMessageBufferClear";
            this.bMessageBufferClear.Size = new System.Drawing.Size(59, 19);
            this.bMessageBufferClear.TabIndex = 16;
            this.bMessageBufferClear.Text = "Clear";
            this.bMessageBufferClear.UseVisualStyleBackColor = true;
            this.bMessageBufferClear.Click += new System.EventHandler(this.bMessageBufferClear_Click);
            // 
            // ProgBarLabel
            // 
            this.ProgBarLabel.AutoSize = true;
            this.ProgBarLabel.Location = new System.Drawing.Point(557, 521);
            this.ProgBarLabel.Name = "ProgBarLabel";
            this.ProgBarLabel.Size = new System.Drawing.Size(71, 13);
            this.ProgBarLabel.TabIndex = 17;
            this.ProgBarLabel.Text = "ProgBarLabel";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(392, 516);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(41, 37);
            this.button1.TabIndex = 18;
            this.button1.Text = "Prime\r\nLink\r\n";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // bMessWriteFile
            // 
            this.bMessWriteFile.Location = new System.Drawing.Point(1056, 512);
            this.bMessWriteFile.Name = "bMessWriteFile";
            this.bMessWriteFile.Size = new System.Drawing.Size(89, 19);
            this.bMessWriteFile.TabIndex = 19;
            this.bMessWriteFile.Text = "Save To File";
            this.bMessWriteFile.UseVisualStyleBackColor = true;
            this.bMessWriteFile.Click += new System.EventHandler(this.bMessWriteFile_Click);
            // 
            // Form1
            // 
            this.AcceptButton = this.MyAcceptButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1242, 561);
            this.Controls.Add(this.bMessWriteFile);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.ProgBarLabel);
            this.Controls.Add(this.bMessageBufferClear);
            this.Controls.Add(this.cbMessBuffAutoScroll);
            this.Controls.Add(this.label56);
            this.Controls.Add(this.labMsec);
            this.Controls.Add(this.rtbMessageBuff);
            this.Controls.Add(this.tbMilliseconds);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.MyAcceptButton);
            this.Controls.Add(this.LabAPI);
            this.Controls.Add(this.TBConnectStatus);
            this.Controls.Add(this.tabControl1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.tabMemoryPage.ResumeLayout(false);
            this.tabMemoryPage.PerformLayout();
            this.tabControl1.ResumeLayout(false);
            this.tabMenuPage.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudCurrentChannel)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudIndex)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox TBConnectStatus;
        private System.Windows.Forms.Label LabAPI;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button MyAcceptButton;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.TabPage tabMemoryPage;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox TBMemAddress;
        private System.Windows.Forms.TextBox MemTextBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label H;
        private System.Windows.Forms.Button ButLineHigher;
        private System.Windows.Forms.Button ButPageHigher;
        private System.Windows.Forms.Button ButLineLower;
        private System.Windows.Forms.Button ButPageLower;
        private System.Windows.Forms.TabPage tabMenuPage;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.Label LMenuTitle;
        private System.Windows.Forms.Button EscapeButton;
        private System.Windows.Forms.ListBox MenuListBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton rbSerial;
        private System.Windows.Forms.RadioButton rbUSB;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.RadioButton rbSelectSerial;
        private System.Windows.Forms.RadioButton rbSelectUSB;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown nudIndex;
        private System.Windows.Forms.Button bConnect;
        private System.Windows.Forms.NumericUpDown nudCurrentChannel;
        private System.Windows.Forms.Button bDisconnect;
        private System.Windows.Forms.TextBox tbMilliseconds;
        private System.Windows.Forms.Label labMsec;
        private System.Windows.Forms.RichTextBox rtbMessageBuff;
        private System.Windows.Forms.Label label56;
        private System.Windows.Forms.CheckBox cbMessBuffAutoScroll;
        private System.Windows.Forms.Button bMessageBufferClear;
        private System.Windows.Forms.Label ProgBarLabel;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button bMessWriteFile;
    }
}
