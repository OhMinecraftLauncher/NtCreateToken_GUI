using System.ComponentModel;
using System.Diagnostics;
using System.Globalization;
using System.Resources;
using System.Security.Principal;
using System.Windows.Forms;
using System.Xml.Linq;

namespace NtCreateToken_GUI
{
    public partial class Form1 : Form
    {
        private readonly ComponentResourceManager manager;
        readonly object[][] privs = {
            new object[] { false, "SeCreateTokenPrivilege", "" },
            new object[] { false, "SeAssignPrimaryTokenPrivilege", "" },
            new object[] { false, "SeLockMemoryPrivilege", "" },
            new object[] { false, "SeIncreaseQuotaPrivilege", "" },
            new object[] { false, "SeMachineAccountPrivilege", "" },
            new object[] { false, "SeTcbPrivilege", "" },
            new object[] { false, "SeSecurityPrivilege", "" },
            new object[] { false, "SeTakeOwnershipPrivilege", "" },
            new object[] { false, "SeLoadDriverPrivilege", "" },
            new object[] { false, "SeSystemProfilePrivilege", "" },
            new object[] { false, "SeSystemtimePrivilege", "" },
            new object[] { false, "SeProfileSingleProcessPrivilege", "" },
            new object[] { false, "SeIncreaseBasePriorityPrivilege", "" },
            new object[] { false, "SeCreatePagefilePrivilege", "" },
            new object[] { false, "SeCreatePermanentPrivilege", "" },
            new object[] { false, "SeBackupPrivilege", "" },
            new object[] { false, "SeRestorePrivilege", "" },
            new object[] { false, "SeShutdownPrivilege", "" },
            new object[] { false, "SeDebugPrivilege", "" },
            new object[] { false, "SeAuditPrivilege", "" },
            new object[] { false, "SeSystemEnvironmentPrivilege", "" },
            new object[] { false, "SeChangeNotifyPrivilege", "" },
            new object[] { false, "SeRemoteShutdownPrivilege", "" },
            new object[] { false, "SeUndockPrivilege", "" },
            new object[] { false, "SeSyncAgentPrivilege", "" },
            new object[] { false, "SeEnableDelegationPrivilege", "" },
            new object[] { false, "SeManageVolumePrivilege", "" },
            new object[] { false, "SeImpersonatePrivilege", "" },
            new object[] { false, "SeCreateGlobalPrivilege", "" },
            new object[] { false, "SeTrustedCredManAccessPrivilege", "" },
            new object[] { false, "SeRelabelPrivilege", "" },
            new object[] { false, "SeIncreaseWorkingSetPrivilege", "" },
            new object[] { false, "SeTimeZonePrivilege", "" },
            new object[] { false, "SeCreateSymbolicLinkPrivilege", "" },
            new object[] { false, "SeDelegateSessionUserImpersonatePrivilege", "" },
            //new object[] { false, "SeUnsolicitedInputPrivilege", "£®≤ªø…”√£©" },
        };
        readonly object[][] groups = {
            new object[] { "S-1-5-32-544", "",true,true,true,true,false,false,false,false,false },
            new object[] { "S-1-5-11", "", true,true,true,false,false,false,false,false,false },
            new object[] { "S-1-1-0", "", true,true,true,false,false,false,false,false,false },
            new object[] { "S-1-16-16384", "", false,false,false,false,false,false,false,true,true },
            new object[] { "S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464", "", true,true,true,true,false,false,false,false,false },
            new object[] { "S-1-5-80-30551196-2029750602-3680353947-2336859763-523537544", "", false,true,true,true,false,false,false,false,false },
            new object[] { "S-1-5-80-3232712927-1625117661-2590453128-1738570065-3637376297", "", false,true,true,true,false,false,false,false,false },
            new object[] { "S-1-5-80-1913148863-3492339771-4165695881-2087618961-4109116736", "", false,true,true,true,false,false,false,false,false },
            new object[] { "S-1-5-80-259296475-4084429506-1152984619-38739575-565535606", "", false,true,true,true,false,false,false,false,false },
            new object[] { "S-1-5-80-1523878533-411328482-2798077809-3098663872-2604013308", "", false,true,true,true,false,false,false,false,false },
            new object[] { "S-1-5-80-3668810961-2468724468-4084584310-3029221373-430494444", "", false,true,true,true,false,false,false,false,false },
            new object[] { "S-1-5-80-3088073201-1464728630-1879813800-1107566885-823218052", "", false,true,true,true,false,false,false,false,false },
            new object[] { "S-1-5-80-1383147646-27650227-2710666058-1662982300-1023958487", "", false,true,true,true,false,false,false,false,false },
        };
        readonly Dictionary<string, string> integrityLevels = new()
        {
            { "S-1-16-0", "Untrusted Mandatory Level" },
            { "S-1-16-4096", "Low Mandatory Level" },
            { "S-1-16-8192", "Medium Mandatory Level" },
            { "S-1-16-8448", "Medium Plus Mandatory Level" },
            { "S-1-16-12288", "High Mandatory Level" },
            { "S-1-16-16384", "System Mandatory Level" },
            { "S-1-16-20480", "Protected Process Mandatory Level" }
        };
        readonly string[] group_attr = {
            "",
            "",
            "SE_GROUP_MANDATORY",
            "SE_GROUP_ENABLED_BY_DEFAULT",
            "SE_GROUP_ENABLED",
            "SE_GROUP_OWNER",
            "SE_GROUP_USE_FOR_DENY_ONLY",
            "SE_GROUP_RESOURCE",
            "SE_GROUP_LOGON_ID",
            "SE_GROUP_INTEGRITY",
            "SE_GROUP_INTEGRITY_ENABLED"
        };
        public string GetNameFromSID(string sid)
        {
            try
            {
                SecurityIdentifier identifier = new(sid);
                NTAccount ntAccount = (NTAccount)identifier.Translate(typeof(NTAccount));
                return ntAccount.ToString();
            }
            catch
            {
                if (integrityLevels.TryGetValue(sid, out string? value))
                {
                    if (value != null) return @"Mandatory Label\" + value;
                }
                return manager.GetString("SidUnknown") ?? "";
            }
        }
        public Form1()
        {
            string installedUICulture = CultureInfo.CurrentUICulture.Name;
            if (installedUICulture.Contains("zh")) Thread.CurrentThread.CurrentUICulture = new("zh");
            else Thread.CurrentThread.CurrentUICulture = new("en");
            manager = new(typeof(Strings));
            InitializeComponent();
            foreach (object[] priv in privs)
            {
                priv[2] = manager.GetString((string)priv[1]) ?? "";
                privTable.Rows.Add(priv);
            }
            foreach (object[] group in groups)
            {
                group[1] = GetNameFromSID((string)group[0]);
                groupTable.Rows.Add(group);
            }
            foreach (DataGridViewColumn a in privTable.Columns)
            {
                a.AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
            }
            foreach (DataGridViewColumn a in groupTable.Columns)
            {
                a.AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
            }
            label3.Text = GetNameFromSID(textBox2.Text);
        }

        private void privTable_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                DataGridViewCheckBoxCell cell = (DataGridViewCheckBoxCell)
                privTable.Rows[e.RowIndex].Cells[e.ColumnIndex];
                cell.Value = !(bool)cell.Value;
            }
            catch { }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            new AddPriv().ShowDialog(this);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow a in privTable.Rows)
            {
                a.Cells[0].Value = true;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow a in privTable.Rows)
            {
                a.Cells[0].Value = false;
            }
        }

        private void groupTable_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                DataGridViewCheckBoxCell cell = (DataGridViewCheckBoxCell)
                groupTable.Rows[e.RowIndex].Cells[e.ColumnIndex];
                cell.Value = !(bool)cell.Value;
            }
            catch { }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            new AddGroup().ShowDialog(this);
        }

        private void button5_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewCell cell in groupTable.SelectedCells)
            {
                try
                {
                    groupTable.Rows.RemoveAt(cell.RowIndex);
                }
                catch { }
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            if (textBox2.Text == "") textBox2.Text = "S-1-5-18";
            label3.Text = GetNameFromSID(textBox2.Text);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new()
            {
                Filter = (manager.GetString("ExecutableFile") ?? "") + " (*.exe;*.bat;*.cmd;*.msi;*.com)|*.exe;*.bat;*.cmd;*.msi;*.com|" + (manager.GetString("AllFiles") ?? "") + " (*.*)|*.*",
                FilterIndex = 1,
                Title = manager.GetString("ExecutableTitle") ?? "",
                RestoreDirectory = true,
                CheckFileExists = true,
                CheckPathExists = true,
                Multiselect = false
            };
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = openFileDialog.FileName;
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (textBox1.Text == "") textBox1.Text = "C:\\Windows\\System32\\cmd.exe";
        }

        public static bool IsAdministrator()
        {
            WindowsIdentity identity = WindowsIdentity.GetCurrent();
            WindowsPrincipal principal = new WindowsPrincipal(identity);
            return principal.IsInRole(WindowsBuiltInRole.Administrator);
        }

        private void button7_Click(object sender, EventArgs e)
        {
            string arg = "/n /c \"";
            arg += textBox1.Text + "\" /s \"";
            arg += textBox2.Text + "\" /g \"";
            foreach (DataGridViewRow row in groupTable.Rows)
            {
                arg += (string)row.Cells[0].Value + ',';
                for (int i = 2; i <= 10; i++)
                {
                    if ((bool)row.Cells[i].Value)
                    {
                        arg += group_attr[i] + '*';
                    }
                }
                if (arg.Last() == '*') arg = arg.Remove(arg.Length - 1);
                arg += ';';
            }
            if (arg.Last() == ';') arg = arg.Remove(arg.Length - 1);
            arg += "\" /p \"";
            foreach (DataGridViewRow row in privTable.Rows)
            {
                arg += (string)row.Cells[1].Value;
                if ((bool)row.Cells[0].Value) arg += ":N";
                arg += ';';
            }
            if (arg.Last() == ';') arg = arg.Remove(arg.Length - 1);
            arg += '\"';
            DialogResult result = MessageBox.Show((manager.GetString("BeforeRun1") ?? "") + "\n\nNtCreateToken.exe " + arg + "\n\n" + (manager.GetString("BeforeRun2") ?? "") + "\n" + (manager.GetString("BeforeRun3") ?? "") + "\n" + (manager.GetString("BeforeRun4") ?? "") + "\n" + (manager.GetString("BeforeRun5") ?? ""), manager.GetString("Warning") ?? "", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);
            if (result == DialogResult.Yes)
            {
                if (!IsAdministrator())
                {
                    MessageBox.Show((manager.GetString("FailToRun") ?? "") + "\n" + (manager.GetString("AdminRun") ?? ""), manager.GetString("Error") ?? "", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                if (File.Exists("NtCreateToken.exe"))
                {
                    Process p = new()
                    {
                        StartInfo = new()
                        {
                            FileName = "NtCreateToken.exe",
                            Arguments = arg,
                            CreateNoWindow = true,
                            UseShellExecute = false,
                            RedirectStandardOutput = true,
                        }
                    };
                    if (p.Start())
                    {
                        p.WaitForExit();
                        if (p.ExitCode == 0)
                        {
                            MessageBox.Show(manager.GetString("SuccessRun") ?? "", manager.GetString("Success") ?? "", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        }
                        else
                        {
                            string err = p.StandardOutput.ReadToEnd();
                            MessageBox.Show((manager.GetString("FailToRun") ?? "") + (manager.GetString("CheckArg") ?? "") + "\n" + err, manager.GetString("Error") ?? "", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        }
                    }
                    else
                    {
                        MessageBox.Show((manager.GetString("FailToRun") ?? "") + (manager.GetString("CheckArg") ?? ""), manager.GetString("Error") ?? "", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
                else
                {
                    MessageBox.Show(manager.GetString("FileMissing") ?? "", manager.GetString("Error") ?? "", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else if (result == DialogResult.No)
            {
                try
                {
                    Clipboard.SetText("NtCreateToken.exe " + arg);
                    MessageBox.Show(manager.GetString("CopyCSucc") ?? "", manager.GetString("Success") ?? "", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                catch (Exception ex)
                {
                    MessageBox.Show((manager.GetString("FailToCopyC") ?? "") + "\n" + ex, manager.GetString("Error") ?? "", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void button8_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewCell cell in privTable.SelectedCells)
            {
                try
                {
                    privTable.Rows.RemoveAt(cell.RowIndex);
                }
                catch { }
            }
        }

        private void button9_Click(object sender, EventArgs e)
        {
            groupTable.Rows.Clear();
            privTable.Rows.Clear();
            textBox2.Text = "S-1-5-18";
            label3.Text = GetNameFromSID(textBox2.Text);
            textBox1.Text = "C:\\Windows\\System32\\cmd.exe";
            foreach (object[] priv in privs)
            {
                privTable.Rows.Add(priv);
            }
            foreach (object[] group in groups)
            {
                groupTable.Rows.Add(group);
            }
        }

        private string[] ExportText()
        {
            List<string> lines = new()
                {
                    textBox1.Text,
                    textBox2.Text,
                    "Groups:"
                };
            foreach (DataGridViewRow row in groupTable.Rows)
            {
                string l = row.Cells[0].Value + "|";
                for (int i = 2; i <= 10; i++)
                {
                    l += ((bool)row.Cells[i].Value).ToString() + ',';
                }
                l = l.Remove(l.Length - 1);
                lines.Add(l);
            }
            lines.Add("Privileges:");
            foreach (DataGridViewRow row in privTable.Rows)
            {
                lines.Add(row.Cells[1].Value + "|" + row.Cells[2].Value + "|" + ((bool)row.Cells[0].Value).ToString());
            }
            return lines.ToArray();
        }

        private void ImportData(string[] lines)
        {
            groupTable.Rows.Clear();
            privTable.Rows.Clear();
            textBox1.Text = lines[0];
            textBox2.Text = lines[1];
            label3.Text = GetNameFromSID(textBox2.Text);
            bool isP = false;
            for (int i = 3; i < lines.Length; i++)
            {
                if (lines[i] == "Privileges:")
                {
                    isP = true;
                    continue;
                }
                if (isP)
                {
                    List<object> row = new();
                    string[] sp1 = lines[i].Split('|');
                    if (sp1[2] == "True") row.Add(true);
                    else row.Add(false);
                    row.Add(sp1[0]);
                    row.Add(sp1[1]);
                    privTable.Rows.Add(row.ToArray());
                }
                else
                {
                    string[] sp1 = lines[i].Split('|');
                    List<object> row = new() { sp1[0], GetNameFromSID(sp1[0]) };
                    string[] sp2 = sp1[1].Split(',');
                    for (int j = 0; j <= 8; j++)
                    {
                        if (sp2[j] == "True") row.Add(true);
                        else row.Add(false);
                    }
                    groupTable.Rows.Add(row.ToArray());
                }
            }
        }

        private void button10_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog
            {
                Filter = (manager.GetString("NTCTP") ?? "") + " (*.ntctini)|*.ntctini",
                Title = manager.GetString("ExportP") ?? "",
            };

            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                string filePath = saveFileDialog.FileName;
                File.WriteAllLines(filePath, ExportText());
                MessageBox.Show(manager.GetString("ExportSuc") ?? "", manager.GetString("Success") ?? "", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        public void ImportDataFile(string filepath)
        {
            string[] old_Prop = ExportText();
            try
            {
                string[] lines = File.ReadAllLines(filepath);
                ImportData(lines);
                MessageBox.Show(manager.GetString("ImportSuc") ?? "", manager.GetString("Success") ?? "", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show((manager.GetString("ImportFail") ?? "") + "\n" + ex, manager.GetString("Error") ?? "", MessageBoxButtons.OK, MessageBoxIcon.Error);
                ImportData(old_Prop);
            }
        }

        private void button11_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new()
            {
                Filter = (manager.GetString("NTCTP") ?? "") + " (*.ntctini)|*.ntctini|" + (manager.GetString("AllFiles") ?? "") + " (*.*)|*.*",
                Title = manager.GetString("ChooseNTCTP") ?? "",
                FilterIndex = 1,
                RestoreDirectory = true,
                CheckFileExists = true,
                CheckPathExists = true,
                Multiselect = false
            };
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                ImportDataFile(openFileDialog.FileName);
            }
        }
    }
}
