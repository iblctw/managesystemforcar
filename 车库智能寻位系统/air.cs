using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace 车库智能寻位系统
{
    public partial class air : Form
    {
        public air()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (richTextBox1.Text.Equals(""))
            {
                MessageBox.Show("请输入需要广播的文字", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
            }
            else
            {
                if (richTextBox1.Text.Trim().Length > 80)
                {
                    MessageBox.Show("已超过80个字，请斟酌精简文字", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                }
                else
                {
                    dd.oldmessege = dd.newmessege;
                    dd.newmessege = richTextBox1.Text.Trim();
                    MessageBox.Show("广播成功", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                }
            }
            
        }
        /// <summary>
        /// //////////字数显示////////////////////////////////
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
            label3.Text = richTextBox1.Text.Trim().Length.ToString();
            if (richTextBox1.Text.Trim().Length > 80)
            {
                MessageBox.Show("已超过80个字，请斟酌精简文字", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
            }
        }
/// <summary>
/// ///////////////////展开用户反馈界面/////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void pictureBox1_Click(object sender, EventArgs e)
        {
            
                this.Height = 403;
                pictureBox1.Visible = false;
                pictureBox2.Visible = true;

                StreamReader sr = new StreamReader("fankui.txt",System.Text.Encoding.Unicode);
                textBox1.Text = sr.ReadToEnd();
           
        }

        private void pictureBox1_MouseEnter(object sender, EventArgs e)
        {
            label5.Visible = true;
        }

        private void pictureBox1_MouseLeave(object sender, EventArgs e)
        {
            label5.Visible = false;
        }

        private void pictureBox2_Click(object sender, EventArgs e)
        {
            this.Height = 221;
            pictureBox2.Visible = false;
            pictureBox1.Visible = true;
        }
    }
}
