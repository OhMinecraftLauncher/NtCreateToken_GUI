using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NtCreateToken_GUI
{
    public partial class AddPriv : Form
    {
        public AddPriv()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (textBox1.Text == "")
            {
                MessageBox.Show("“名称”输入框不能为空！", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            ((Form1)Owner).privTable.Rows.Add(false, textBox1.Text, textBox2.Text);
            Close();
        }
    }
}
