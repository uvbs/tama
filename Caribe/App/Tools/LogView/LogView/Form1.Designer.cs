namespace LogView
{
    partial class Form1
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다.
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            this.targetinfo = new System.Windows.Forms.TextBox();
            this.searchtype = new System.Windows.Forms.ComboBox();
            this.button_search = new System.Windows.Forms.Button();
            this.listViewLog = new System.Windows.Forms.ListView();
            this.LogIndex = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.IdAccount = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.NickName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.LogType = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Context = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.LogTime = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.SuspendLayout();
            // 
            // targetinfo
            // 
            this.targetinfo.Location = new System.Drawing.Point(148, 11);
            this.targetinfo.MaxLength = 100;
            this.targetinfo.Name = "targetinfo";
            this.targetinfo.Size = new System.Drawing.Size(1023, 21);
            this.targetinfo.TabIndex = 0;
            this.targetinfo.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbxName_KeyDown);
            // 
            // searchtype
            // 
            this.searchtype.FormattingEnabled = true;
            this.searchtype.Items.AddRange(new object[] {
            "NickName",
            "UUID",
            "IDAccount"});
            this.searchtype.Location = new System.Drawing.Point(12, 12);
            this.searchtype.Name = "searchtype";
            this.searchtype.Size = new System.Drawing.Size(130, 20);
            this.searchtype.TabIndex = 1;
            this.searchtype.Text = "All";
            this.searchtype.SelectedIndexChanged += new System.EventHandler(this.searchtype_SelectedIndexChanged);
            // 
            // button_search
            // 
            this.button_search.Location = new System.Drawing.Point(1177, 9);
            this.button_search.Name = "button_search";
            this.button_search.Size = new System.Drawing.Size(75, 23);
            this.button_search.TabIndex = 2;
            this.button_search.Text = "Search";
            this.button_search.UseVisualStyleBackColor = true;
            this.button_search.Click += new System.EventHandler(this.button_search_Click);
            // 
            // listViewLog
            // 
            this.listViewLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listViewLog.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.LogIndex,
            this.IdAccount,
            this.NickName,
            this.LogType,
            this.Context,
            this.LogTime});
            this.listViewLog.FullRowSelect = true;
            this.listViewLog.GridLines = true;
            this.listViewLog.LabelEdit = true;
            this.listViewLog.Location = new System.Drawing.Point(12, 38);
            this.listViewLog.Name = "listViewLog";
            this.listViewLog.Size = new System.Drawing.Size(1240, 870);
            this.listViewLog.TabIndex = 3;
            this.listViewLog.UseCompatibleStateImageBehavior = false;
            this.listViewLog.View = System.Windows.Forms.View.Details;
            this.listViewLog.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.listViewLog_ColumnClick);
            // 
            // LogIndex
            // 
            this.LogIndex.Text = "LogIndex";
            this.LogIndex.Width = 80;
            // 
            // IdAccount
            // 
            this.IdAccount.Text = "IdAccount";
            this.IdAccount.Width = 80;
            // 
            // NickName
            // 
            this.NickName.Text = "NickName";
            this.NickName.Width = 110;
            // 
            // LogType
            // 
            this.LogType.Text = "LogType";
            this.LogType.Width = 103;
            // 
            // Context
            // 
            this.Context.Text = "Context";
            this.Context.Width = 696;
            // 
            // LogTime
            // 
            this.LogTime.Text = "LogTime";
            this.LogTime.Width = 134;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(1264, 921);
            this.Controls.Add(this.listViewLog);
            this.Controls.Add(this.button_search);
            this.Controls.Add(this.searchtype);
            this.Controls.Add(this.targetinfo);
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox targetinfo;
        private System.Windows.Forms.ComboBox searchtype;
        private System.Windows.Forms.Button button_search;
        private System.Windows.Forms.ListView listViewLog;
        private System.Windows.Forms.ColumnHeader LogIndex;
        private System.Windows.Forms.ColumnHeader NickName;
        private System.Windows.Forms.ColumnHeader LogType;
        private System.Windows.Forms.ColumnHeader Context;
        private System.Windows.Forms.ColumnHeader LogTime;
        private System.Windows.Forms.ColumnHeader IdAccount;
    }
}

