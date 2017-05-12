using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace 车库智能寻位系统
{
    public partial class go : Form
    {
            
        public go()
        {
            InitializeComponent();
            
        }
/// <summary>
/// /////////////////////实时刷新车位占用情况
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void timer1_Tick(object sender, EventArgs e)
        {
            //if (dd.gzt == 0)                          //显示杆状态
            //{
            //    pictureBox14.Visible = false;
            //    pictureBox13.Visible = true;
            //}
            //else
            //{
            //    pictureBox14.Visible = true;
            //    pictureBox13.Visible = false;
            //}

            for (int i=1; i <13; i++)                 //显示每个车位的状态
            {
                if (dd.cwzt[i] == 1)
                {
                    switch (i)
                    {
                        case 1:
                            pictureBox1.Visible = true;
                            break;
                        case 2:
                            pictureBox2.Visible = true;
                            break;
                        case 3:
                            pictureBox3.Visible = true;
                            break;
                        case 4:
                            pictureBox4.Visible = true;
                            break;
                        case 5:
                            pictureBox5.Visible = true;
                            break;
                        case 6:
                            pictureBox6.Visible = true;
                            break;
                        case 7:
                            pictureBox7.Visible = true;
                            break;
                        case 8:
                            pictureBox8.Visible = true;
                            break;
                        case 9:
                            pictureBox9.Visible = true;
                            break;
                        case 10:
                            pictureBox10.Visible = true;
                            break;
                        case 11:
                            pictureBox11.Visible = true;
                            break;
                        case 12:
                            pictureBox12.Visible = true;
                            break;
                    }
                }
                else
                {
                    switch (i)
                    {
                        case 1:
                            pictureBox1.Visible = false;
                            break;
                        case 2:
                            pictureBox2.Visible = false;
                            break;
                        case 3:
                            pictureBox3.Visible = false;
                            break;
                        case 4:
                            pictureBox4.Visible = false;
                            break;
                        case 5:
                            pictureBox5.Visible = false;
                            break;
                        case 6:
                            pictureBox6.Visible = false;
                            break;
                        case 7:
                            pictureBox7.Visible = false;
                            break;
                        case 8:
                            pictureBox8.Visible = false;
                            break;
                        case 9:
                            pictureBox9.Visible = false;
                            break;
                        case 10:
                            pictureBox10.Visible = false;
                            break;
                        case 11:
                            pictureBox11.Visible = false;
                            break;
                        case 12:
                            pictureBox12.Visible = false;
                            break;
                    }
                }
 
            }

        }

        /// <summary>
        /// ///////////点击出入口的门杆手动控制升落
        /// </summary>
        /// <param name = "sender" ></ param >
        /// < param name="e"></param>
          
        //private void pictureBox13_Click(object sender, EventArgs e)
        //{
        //    dd.gzt = 1;          //将杆状态置为抬起
        //    pictureBox14.Visible = true;
        //    pictureBox13.Visible = false;
        //}


        ////////////////////////////////////////////////////////////

        /// <summary>
        /// /////////////////////鼠标移动到已被占用的车位上时显示车位信息///////////
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void pictureBox3_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[3] != null)
            {
                label3.Text = dd.xuanwei[3];
            }
            label1.Text = "03";
            panel1.Visible = true;
        }

        private void pictureBox3_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false;
        }

        private void pictureBox1_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[1] != null)
            {
                label3.Text = dd.xuanwei[1];
            }
            label1.Text = "01";
            panel1.Visible = true;
        }

        private void pictureBox1_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false;
        }

        private void pictureBox2_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[2] != null)
            {
                label3.Text = dd.xuanwei[2];
            }
            label1.Text = "02";
            panel1.Visible = true;
        }

        private void pictureBox2_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false;
        }

        private void pictureBox4_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[4] != null)
            {
                label3.Text = dd.xuanwei[4];
            }
            label1.Text = "04";
            panel1.Visible = true;
        }

        private void pictureBox4_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false;
        }

        private void pictureBox5_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[5] != null)
            {
                label3.Text = dd.xuanwei[5];
            }
            label1.Text = "05";
            panel1.Visible = true;
        }

        private void pictureBox5_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false;
        }

        private void pictureBox6_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[6] != null)
            {
                label3.Text = dd.xuanwei[6];
            }
            label1.Text = "06";
            panel1.Visible = true;
        }

        private void pictureBox6_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false;
        }

        private void pictureBox7_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[7] != null)
            {
                label3.Text = dd.xuanwei[7];
            }
            label1.Text = "07";
            panel1.Visible = true;
        }

        private void pictureBox7_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false;
        }

        private void pictureBox8_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[8] != null)
            {
                label3.Text = dd.xuanwei[8];
            }
            label1.Text = "08";
            panel1.Visible = true;
        }

        private void pictureBox8_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false;
        }

        private void pictureBox9_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[9] != null)
            {
                label3.Text = dd.xuanwei[9];
            }
            label1.Text = "09";
            panel1.Visible = true;
        }

        private void pictureBox9_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false ;
        }

        private void pictureBox10_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[10] != null)
            {
                label3.Text = dd.xuanwei[10];
            }
            label1.Text = "10";
            panel1.Visible = true;
        }

        private void pictureBox10_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false ;
        }

        private void pictureBox11_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[11] != null)
            {
                label3.Text = dd.xuanwei[11];
            }
            label1.Text = "11";
            panel1.Visible = true;
        }

        private void pictureBox11_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false ;
        }

        private void pictureBox12_MouseEnter(object sender, EventArgs e)
        {
            if (dd.xuanwei[12] != null)
            {
                label3.Text = dd.xuanwei[12];
            }
            label1.Text = "12";
            panel1.Visible = true;
        }

        private void pictureBox12_MouseLeave(object sender, EventArgs e)
        {
            panel1.Visible = false ;
        }

        private void pictureBox9_Click(object sender, EventArgs e)
        {

        }

        private void pictureBox8_Click(object sender, EventArgs e)
        {

        }



        ////////////////////////////////////////////////////////////////////////////
    }
}
