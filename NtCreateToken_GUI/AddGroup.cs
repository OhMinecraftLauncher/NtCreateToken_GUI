using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace NtCreateToken_GUI
{
    public partial class AddGroup : Form
    {
        public AddGroup()
        {
            InitializeComponent();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            label2.Text = ((Form1)Owner).GetNameFromSID(textBox1.Text);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (textBox1.Text == "")
            {
                MessageBox.Show("“SID”输入框不能为空！", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            ((Form1)Owner).groupTable.Rows.Add(textBox1.Text, ((Form1)Owner).GetNameFromSID(textBox1.Text),false,true,true,true,false,false,false,false,false);
            Close();
        }
    }
}
