namespace NtCreateToken_GUI
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            button1 = new Button();
            privTable = new DataGridView();
            Disable = new DataGridViewCheckBoxColumn();
            PName = new DataGridViewTextBoxColumn();
            Content = new DataGridViewTextBoxColumn();
            groupTable = new DataGridView();
            dataGridViewTextBoxColumn1 = new DataGridViewTextBoxColumn();
            dataGridViewTextBoxColumn2 = new DataGridViewTextBoxColumn();
            MANDATORY = new DataGridViewCheckBoxColumn();
            DEFAULT = new DataGridViewCheckBoxColumn();
            ENABLED = new DataGridViewCheckBoxColumn();
            OWNER = new DataGridViewCheckBoxColumn();
            USE_FOR_DENY_ONLY = new DataGridViewCheckBoxColumn();
            RESOURCE = new DataGridViewCheckBoxColumn();
            LOGON_ID = new DataGridViewCheckBoxColumn();
            INTEGRITY = new DataGridViewCheckBoxColumn();
            INTEGRITY_ENABLED = new DataGridViewCheckBoxColumn();
            button2 = new Button();
            button3 = new Button();
            button4 = new Button();
            button5 = new Button();
            label1 = new Label();
            textBox1 = new TextBox();
            button6 = new Button();
            label2 = new Label();
            textBox2 = new TextBox();
            label3 = new Label();
            button7 = new Button();
            button8 = new Button();
            button9 = new Button();
            button10 = new Button();
            button11 = new Button();
            ((System.ComponentModel.ISupportInitialize)privTable).BeginInit();
            ((System.ComponentModel.ISupportInitialize)groupTable).BeginInit();
            SuspendLayout();
            // 
            // button1
            // 
            resources.ApplyResources(button1, "button1");
            button1.Name = "button1";
            button1.UseVisualStyleBackColor = true;
            button1.Click += button1_Click;
            // 
            // privTable
            // 
            resources.ApplyResources(privTable, "privTable");
            privTable.AllowUserToAddRows = false;
            privTable.AllowUserToDeleteRows = false;
            privTable.AllowUserToResizeColumns = false;
            privTable.AllowUserToResizeRows = false;
            privTable.Columns.AddRange(new DataGridViewColumn[] { Disable, PName, Content });
            privTable.Name = "privTable";
            privTable.ReadOnly = true;
            privTable.RowHeadersVisible = false;
            privTable.CellContentClick += privTable_CellContentClick;
            // 
            // Disable
            // 
            resources.ApplyResources(Disable, "Disable");
            Disable.Name = "Disable";
            Disable.ReadOnly = true;
            // 
            // PName
            // 
            resources.ApplyResources(PName, "PName");
            PName.Name = "PName";
            PName.ReadOnly = true;
            // 
            // Content
            // 
            resources.ApplyResources(Content, "Content");
            Content.Name = "Content";
            Content.ReadOnly = true;
            // 
            // groupTable
            // 
            resources.ApplyResources(groupTable, "groupTable");
            groupTable.AllowUserToAddRows = false;
            groupTable.AllowUserToDeleteRows = false;
            groupTable.AllowUserToResizeColumns = false;
            groupTable.AllowUserToResizeRows = false;
            groupTable.Columns.AddRange(new DataGridViewColumn[] { dataGridViewTextBoxColumn1, dataGridViewTextBoxColumn2, MANDATORY, DEFAULT, ENABLED, OWNER, USE_FOR_DENY_ONLY, RESOURCE, LOGON_ID, INTEGRITY, INTEGRITY_ENABLED });
            groupTable.Name = "groupTable";
            groupTable.ReadOnly = true;
            groupTable.RowHeadersVisible = false;
            groupTable.CellContentClick += groupTable_CellContentClick;
            // 
            // dataGridViewTextBoxColumn1
            // 
            resources.ApplyResources(dataGridViewTextBoxColumn1, "dataGridViewTextBoxColumn1");
            dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            dataGridViewTextBoxColumn1.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn2
            // 
            resources.ApplyResources(dataGridViewTextBoxColumn2, "dataGridViewTextBoxColumn2");
            dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            dataGridViewTextBoxColumn2.ReadOnly = true;
            // 
            // MANDATORY
            // 
            resources.ApplyResources(MANDATORY, "MANDATORY");
            MANDATORY.Name = "MANDATORY";
            MANDATORY.ReadOnly = true;
            // 
            // DEFAULT
            // 
            resources.ApplyResources(DEFAULT, "DEFAULT");
            DEFAULT.Name = "DEFAULT";
            DEFAULT.ReadOnly = true;
            // 
            // ENABLED
            // 
            resources.ApplyResources(ENABLED, "ENABLED");
            ENABLED.Name = "ENABLED";
            ENABLED.ReadOnly = true;
            // 
            // OWNER
            // 
            resources.ApplyResources(OWNER, "OWNER");
            OWNER.Name = "OWNER";
            OWNER.ReadOnly = true;
            // 
            // USE_FOR_DENY_ONLY
            // 
            resources.ApplyResources(USE_FOR_DENY_ONLY, "USE_FOR_DENY_ONLY");
            USE_FOR_DENY_ONLY.Name = "USE_FOR_DENY_ONLY";
            USE_FOR_DENY_ONLY.ReadOnly = true;
            USE_FOR_DENY_ONLY.Resizable = DataGridViewTriState.True;
            USE_FOR_DENY_ONLY.SortMode = DataGridViewColumnSortMode.Automatic;
            // 
            // RESOURCE
            // 
            resources.ApplyResources(RESOURCE, "RESOURCE");
            RESOURCE.Name = "RESOURCE";
            RESOURCE.ReadOnly = true;
            // 
            // LOGON_ID
            // 
            resources.ApplyResources(LOGON_ID, "LOGON_ID");
            LOGON_ID.Name = "LOGON_ID";
            LOGON_ID.ReadOnly = true;
            // 
            // INTEGRITY
            // 
            resources.ApplyResources(INTEGRITY, "INTEGRITY");
            INTEGRITY.Name = "INTEGRITY";
            INTEGRITY.ReadOnly = true;
            // 
            // INTEGRITY_ENABLED
            // 
            resources.ApplyResources(INTEGRITY_ENABLED, "INTEGRITY_ENABLED");
            INTEGRITY_ENABLED.Name = "INTEGRITY_ENABLED";
            INTEGRITY_ENABLED.ReadOnly = true;
            // 
            // button2
            // 
            resources.ApplyResources(button2, "button2");
            button2.Name = "button2";
            button2.UseVisualStyleBackColor = true;
            button2.Click += button2_Click;
            // 
            // button3
            // 
            resources.ApplyResources(button3, "button3");
            button3.Name = "button3";
            button3.UseVisualStyleBackColor = true;
            button3.Click += button3_Click;
            // 
            // button4
            // 
            resources.ApplyResources(button4, "button4");
            button4.Name = "button4";
            button4.UseVisualStyleBackColor = true;
            button4.Click += button4_Click;
            // 
            // button5
            // 
            resources.ApplyResources(button5, "button5");
            button5.Name = "button5";
            button5.UseVisualStyleBackColor = true;
            button5.Click += button5_Click;
            // 
            // label1
            // 
            resources.ApplyResources(label1, "label1");
            label1.Name = "label1";
            // 
            // textBox1
            // 
            resources.ApplyResources(textBox1, "textBox1");
            textBox1.Name = "textBox1";
            textBox1.TextChanged += textBox1_TextChanged;
            // 
            // button6
            // 
            resources.ApplyResources(button6, "button6");
            button6.Name = "button6";
            button6.UseVisualStyleBackColor = true;
            button6.Click += button6_Click;
            // 
            // label2
            // 
            resources.ApplyResources(label2, "label2");
            label2.Name = "label2";
            // 
            // textBox2
            // 
            resources.ApplyResources(textBox2, "textBox2");
            textBox2.Name = "textBox2";
            textBox2.TextChanged += textBox2_TextChanged;
            // 
            // label3
            // 
            resources.ApplyResources(label3, "label3");
            label3.AutoEllipsis = true;
            label3.Name = "label3";
            // 
            // button7
            // 
            resources.ApplyResources(button7, "button7");
            button7.Name = "button7";
            button7.UseVisualStyleBackColor = true;
            button7.Click += button7_Click;
            // 
            // button8
            // 
            resources.ApplyResources(button8, "button8");
            button8.Name = "button8";
            button8.UseVisualStyleBackColor = true;
            button8.Click += button8_Click;
            // 
            // button9
            // 
            resources.ApplyResources(button9, "button9");
            button9.Name = "button9";
            button9.UseVisualStyleBackColor = true;
            button9.Click += button9_Click;
            // 
            // button10
            // 
            resources.ApplyResources(button10, "button10");
            button10.Name = "button10";
            button10.UseVisualStyleBackColor = true;
            button10.Click += button10_Click;
            // 
            // button11
            // 
            resources.ApplyResources(button11, "button11");
            button11.Name = "button11";
            button11.UseVisualStyleBackColor = true;
            button11.Click += button11_Click;
            // 
            // Form1
            // 
            resources.ApplyResources(this, "$this");
            AutoScaleMode = AutoScaleMode.Font;
            Controls.Add(button11);
            Controls.Add(button10);
            Controls.Add(button9);
            Controls.Add(button8);
            Controls.Add(button7);
            Controls.Add(label3);
            Controls.Add(textBox2);
            Controls.Add(label2);
            Controls.Add(button6);
            Controls.Add(textBox1);
            Controls.Add(label1);
            Controls.Add(button5);
            Controls.Add(button4);
            Controls.Add(button3);
            Controls.Add(button2);
            Controls.Add(groupTable);
            Controls.Add(privTable);
            Controls.Add(button1);
            MaximizeBox = false;
            Name = "Form1";
            ((System.ComponentModel.ISupportInitialize)privTable).EndInit();
            ((System.ComponentModel.ISupportInitialize)groupTable).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion
        private Button button1;
        internal DataGridView privTable;
        private Button button2;
        private Button button3;
        private Button button4;
        internal DataGridView groupTable;
        private Button button5;
        private Label label1;
        private TextBox textBox1;
        private Button button6;
        private Label label2;
        private TextBox textBox2;
        private Label label3;
        private Button button7;
        private Button button8;
        private Button button9;
        private Button button10;
        private Button button11;
        private DataGridViewCheckBoxColumn Disable;
        private DataGridViewTextBoxColumn PName;
        private DataGridViewTextBoxColumn Content;
        private DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private DataGridViewCheckBoxColumn MANDATORY;
        private DataGridViewCheckBoxColumn DEFAULT;
        private DataGridViewCheckBoxColumn ENABLED;
        private DataGridViewCheckBoxColumn OWNER;
        private DataGridViewCheckBoxColumn USE_FOR_DENY_ONLY;
        private DataGridViewCheckBoxColumn RESOURCE;
        private DataGridViewCheckBoxColumn LOGON_ID;
        private DataGridViewCheckBoxColumn INTEGRITY;
        private DataGridViewCheckBoxColumn INTEGRITY_ENABLED;
    }
}
