using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace 车库智能寻位系统
{
    public partial class jiankong : Form
    {
        int i=0;
        
       
        public jiankong()
        {
            InitializeComponent();
                  
        }

              
/// <summary>
/// /////////////////////////////不断刷新显示的数据///////////////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
       public void timer1_Tick(object sender, EventArgs e)
        {
        
            for (i = 1; i < 13; i++)
            {
                if (dd.xuanwei[i] != null)         //若该车位有用户选择
                {
                    if ((dataGridView1.Rows[i - 1].Cells[2].Value != null) && (dataGridView1.Rows[i - 1].Cells[2].Value.Equals(1)))  //若是已经将车停在车位上
                    {

                        dataGridView1.Rows[i - 1].Cells[1].Value = dd.xuanwei[i];
                        dd.yysj[i] = 0;    //清除预约计时
                    }
                    else                                                                       //若还未停车
                    {
                        dataGridView1.Rows[i - 1].Cells[1].Value = dd.xuanwei[i] + "(预约)";
                    //    dataGridView1.Refresh();
                        dd.yysj[i]++;       //   预约计时
                        if (dd.yysj[i] == 1000)      //十分钟后仍没停车则取消预约
                        {
                            dd.yysj[i] = 0;    //清除预约计时
                            dd.xuanwei[i] = null;
                        }
                    }
                }
                else                                                   //取消预约后清除车主名，清除预约标志
                {
                    dataGridView1.Rows[i - 1].Cells[1].Value = null;
                    dd.yysj[i] = 0;           //清除预约计时
                }
            }
            if (dd.data != null)
            {
                if (dd.data[2] == "0000")
                {
                    dataGridView1.Rows[Convert.ToInt32(dd.data[0]) - 1].Cells[2].Value = Convert.ToInt32(dd.data[1]);      
                    if (Convert.ToInt32(dd.data[1]) == 1)                                                                  //若车停靠车位
                    {
                        dataGridView1.Rows[Convert.ToInt32(dd.data[0]) - 1].Cells[3].Value = main.jin;
                        dd.yysj[Convert.ToInt32(dd.data[0]) - 1] = 0;                        //清除预约计时
                    }
                    else                                                                                         //若车离开
                    {
                        dataGridView1.Rows[Convert.ToInt32(dd.data[0]) - 1].Cells[3].Value=null;
                        dataGridView1.Rows[Convert.ToInt32(dd.data[0]) - 1].Cells[1].Value = null;
                        dd.data = null;
                    }
                }
            }
            
        }



        class Student                         //定义一个空的类，获取空的存储空间 
        {
        } 

        private void jiankong_Load(object sender, EventArgs e)
        {
        //    dd.data = null;               //防止界面启动时就显示上一次存储的数据，但如果这句执行在刚刚写入数据库之前会导致当前数据被删除
            ////初始化每个车位的余额//////////////////

           
            dataGridView1.DataSource = new List<Student>()  //单独声明个20行的存储空间
              {
              new Student() {},new Student() {},new Student() {},new Student() {},new Student() {},
              new Student() {},new Student() {},new Student() {},new Student() {},new Student() {},
              new Student() {},new Student() {},
               };

            for (int a = 0; a < 12; a++)
            {
                
                dataGridView1.Rows[a].Cells[0].Value = a + 1;             //初始给前十二行赋值
            }

        }

        private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

                     
        
    }
}
