using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Data.SqlClient;
using System.Net;
using System.Net.Sockets;
using System.Threading;


namespace 车库智能寻位系统
{
    public partial class main : Form
    {
        
        public static DateTime jin;  //用于向实时监控窗口传输停靠时间
        static int i;             //存储车位号
        public static DateTime rukusj;   //存储入库时间
        public static int yuer;       //存储账户余额
        int gjs = 0;                //杆抬起的时间
        private jiankong s = null;
        private history s1 = null;
        private guanliyuan u = null;
        private go daohang = null;
        private yonghuguanli yhgl = null;
        private air bo = null;
        int state;                //标记窗口是否在屏幕边界隐藏
        int gd = 0;         //用于滚动信息的计时
        int ztsj=2;         //用于设置滚动信息的停留时间，初始欢迎词停留2秒s

                    
        public main()
        {
            InitializeComponent();
            
            Control.CheckForIllegalCrossThreadCalls = false;
           
        }

      
/// <summary>
/// ////////////////////////////////////////////串口数据写入数据库///////////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
 //       public delegate void serrev();   ////定义委托，实现实时显示窗口能响应串口接收事件
  //      public event serrev jie1;

        private void serialPort_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
                                 
            int j;
            dd.data = new string[12];
            string d = serialPort.ReadTo("@");    //接收的字符串以@标志结尾
            dd.data = d.Split('#');            //发送的字符串形式：车位号#占位标识#卡号#出入库标识@，卡号默认值0000
            //string connString = "Data Source=GHX;Database=car;Trusted_Connection=true;";   //定义连接数据库的字符串
            string connString =  dd.sqlConnection;
            SqlConnection connection = new SqlConnection(connString);                            //创建连接对象
      
            /////////////判断接收的数据是刷卡还是停车/////////////
            if (dd.data[2] == "0000")
            {
                i = Convert.ToInt32(dd.data[0]);    ////车位号
                j = Convert.ToInt32(dd.data[1]);    /////占位标识，为1表示停有车
               
           // if(dd.data[2] = "")
                //////////有车停靠时添加记录//////////////////////////////////////////////
                try
                {
                    if (j == 1)                           //若停车前没有选择车位则不录入数据库
                    {
                        if (dd.cw < 7)
                        {
                            dd.cw++;                //已占用车位加一
                        }
                        dd.cwzt[Convert.ToInt32(dd.data[0])] = 1;    //将该车位的占用状态置为1
                        jin = DateTime.Now;
                        string sql = string.Format("insert into history(车位号,车主名,停靠时刻)" + "values('{0}','{1}','{2}')", i, dd.xuanwei[i], DateTime.Now);
                        SqlCommand cmd = new SqlCommand(sql, connection);
                        connection.Open();
                        cmd.ExecuteNonQuery();

                    }
                    /////////////车离开时记录离开时间
                    if (j == 0)
                    {
                        if(dd.cw>0)
                        { 
                            dd.cw--;      //已用车位数减一
                        }
                        dd.cwzt[Convert.ToInt32(dd.data[0])] = 0;    //将该车位的占用状态置为0
                        TimeSpan zt = DateTime.Now - jin;   //离开时刻与停靠时刻相减得到占用时间
                        int zfen = (int)zt.TotalMinutes;
                        SqlCommand chg = new SqlCommand();
                        chg.Connection = connection;
                        string zsj = string.Format("UPDATE history SET 离开时刻='{0}',占用时间='{1}' WHERE 车位号='" + i + "' and 离开时刻 is null", DateTime.Now, zfen);
                        chg.CommandText = zsj;
                        connection.Open();
                        chg.ExecuteNonQuery();

                        //清除预约数据
                        dd.xuanwei[i] = null;

                    }

                }
                catch
                {

                }
            }
            else
            {
                string pdkh = "select count(*) from appusr where 卡号='" + dd.data[2] + "'";  //判断数据库是否存在这个卡号,有则控制单片机抬杆
                SqlCommand cmd = new SqlCommand(pdkh, connection);
                string ye = "select 账户余额 from appusr where 卡号='" + dd.data[2] + "'";    //取出数据库中的余额数
                SqlCommand cmd1 = new SqlCommand(ye, connection);
                connection.Open();

                int n = (int)cmd.ExecuteScalar();
                if (n >= 1)
                {
                    SqlDataReader yue = cmd1.ExecuteReader();
                    yue.Read();
                    yuer = (int)yue["账户余额"];
                    yue.Close();

                    if (dd.data[3] == "1")       //若是入库
                    {

                        if (yuer > 0)           //卡号存在且有余额
                        {
                            string taigan = "01#6";           //向单片机发送抬杆命令
                            dd.gzt = 1;               //将杆状态标志位抬起
                            serialPort.Write(taigan);

                            /////////////将本次的入库时间写入数据库//////////////
                            SqlCommand chg = new SqlCommand();
                            chg.Connection = connection;
                            chg.CommandText = "UPDATE appusr SET 入库时间='" + DateTime.Now + "' WHERE 卡号='" + dd.data[2] + "'";
                            chg.ExecuteNonQuery();

                            rukusj = DateTime.Now;     //记录入库时间

                        }

                        if (yuer <= 0)
                        {
                            MessageBox.Show("该用户余额不足", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                        }
                    }
                    else                  //若是出库
                    {
                        SqlCommand chg1 = new SqlCommand();
                        chg1.Connection = connection;
                        chg1.CommandText = "UPDATE appusr SET 出库时间='" + DateTime.Now + "' WHERE 卡号='" + dd.data[2] + "'";

                        chg1.ExecuteNonQuery();

                        TimeSpan t = DateTime.Now - rukusj;
                        int fen = (int)t.TotalMinutes;            //相减出入库时间得到车主逗留时间

                        yuer = yuer-5;    //根据逗留时间扣除相应金额


                        ///////////更新账户余额//////////
                        SqlCommand gye = new SqlCommand();
                        gye.Connection = connection;
                        gye.CommandText = "UPDATE appusr SET 账户余额='" + yuer + "' WHERE 卡号='" + dd.data[2] + "'";
                        gye.ExecuteNonQuery();

                        if (yuer < 0)                      //减去相应金额后若余额已欠费则不抬杆
                        {
                            MessageBox.Show("该用户余额不足", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                        }
                        else
                        {
                            string taigan = "1";           //向单片机发送抬杆命令
                            dd.gzt = 1;               //将杆状态标志位抬起
                            serialPort.Write(taigan);
                        }
                    }
                }
                else                 //未注册用户不抬杆且提醒管理员
                {


                    MessageBox.Show("该持卡用户尚未注册", "智能车库提醒您", MessageBoxButtons.OK,MessageBoxIcon.Asterisk);
                    

                }
            }
        }
/// <summary>
/// ////////////////////////////////////////////串口设置按钮//////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button1_Click_1(object sender, EventArgs e)
        {
            if (panel1.Visible == true)
            {
                panel1.Visible = false;
            }
            else
            {
                panel1.Visible = true;
                if (serialPort.IsOpen == true)
                {
                    comboBox1.Enabled = false;
                    comboBox2.Enabled = false;
                    comboBox3.Enabled = false;
                    comboBox4.Enabled = false;
                    comboBox5.Enabled = false;
                    button7.Text = "断 开";
                    
                }
               
            }
            
        }
/// <summary>
/// /////////////////////////////////////窗口启动时///////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void onload(object sender, EventArgs e)
        {
            ///////////设置主界面启动位置贴紧屏幕右边缘，距屏幕上边缘十分之一的距离
            this.Left = Screen.PrimaryScreen.WorkingArea.Width - this.Width;
            this.Top = Screen.PrimaryScreen.WorkingArea.Height/10;
            state = 0;

             label13.Text = DateTime.Now.ToString("20yy年MM月dd日 HH:mm:ss");

             Thread jiant = new Thread(new ThreadStart(jianting));           //开启socket监听的后台线程
             jiant.IsBackground = true;
             jiant.Start();

              
        }
/// <summary>
/// //////////////////////////////////创建socket////////////////////////////
/// </summary>
        private void jianting()
        {
            
   /*         int port = 2000;
            string host = "192.168.0.1";

            //创建终结点

            IPAddress ip = IPAddress.Parse(host);

            IPEndPoint ipe = new IPEndPoint(ip,port); 
*/
            IPEndPoint ipe = new IPEndPoint(IPAddress.Any, 2000);

            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp); 
            s.Bind(ipe);    
            s.Listen(10);    //开始监听，设置同时可以监听10个
            while (true)
            {
                try
                {
                    Socket temp = s.Accept();   //为新建立的连接创建新的Socket
                    carsocket newclient = new carsocket(temp);                                //每当监听到客户端连接就开启一个线程
                    Thread newthread = new Thread(new ThreadStart(newclient.ClientServer));
                    newthread.Start();
                }
                catch
                {

                }
            }
        }
/// <summary>
/// //////////////////////////////////////串口连接///////////////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button7_Click(object sender, EventArgs e)
        {
            if (serialPort.IsOpen == true)
            {
                serialPort.Close();
                comboBox1.Enabled = true;
                comboBox2.Enabled = true;
                comboBox3.Enabled = true;
                comboBox4.Enabled = true;
                comboBox5.Enabled = true;
                button7.Text = "连 接";
                label15.Text = " 串口已断开";
                pictureBox1.Visible = false;
                pictureBox2.Visible = true;
            }
            else
            try
            {
               

                serialPort.PortName = comboBox1.SelectedItem.ToString();  //获取串口号（字符串）
                serialPort.BaudRate = Convert.ToInt32(comboBox2.SelectedItem.ToString()); //获取波特率（数字字符）

                //获得选择的校验位(字符串)
                switch (comboBox3.SelectedIndex)
                {
                    case 0:
                        serialPort.Parity = Parity.None;
                        break;
                    case 1:
                        serialPort.Parity = Parity.Odd;
                        break;
                    case 2:
                        serialPort.Parity = Parity.Even;
                        break;
                    case 3:
                        serialPort.Parity = Parity.Mark;
                        break;
                    case 4:
                        serialPort.Parity = Parity.Space;
                        break;
                }

                //数据位（数字字符串）
                serialPort.DataBits = Convert.ToInt32(comboBox4.SelectedItem.ToString());

                //获取停止位
                switch (comboBox5.SelectedIndex)
                {
                    case 0:
                        serialPort.StopBits = StopBits.One;
                        break;
                    case 1:
                        serialPort.StopBits = StopBits.Two;
                        break;
                    case 2:
                        serialPort.StopBits = StopBits.OnePointFive;
                        break;
                }
                if (serialPort.IsOpen == false)
                {
                    serialPort.Open(); //打开串口命令
                    comboBox1.Enabled = false;
                    comboBox2.Enabled = false;
                    comboBox3.Enabled = false;
                    comboBox4.Enabled = false;
                    comboBox5.Enabled = false;
                    label15.Text = serialPort.PortName+"已连接";
                    pictureBox1.Visible = true;
                    pictureBox2.Visible = false;
                    button7.Text = "断 开";
                }
                
                
                panel1.Visible=false;    //将串口设置窗口隐藏
            }
            catch
            {

                MessageBox.Show("串口连接失败！", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
            }
        }
/// <summary>
/// /////////////////////////////////////定时器/////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void timer1_Tick(object sender, EventArgs e)
        {
            if (timer2.Enabled == false)
            {
                gd++;
            }
            if (dd.gzt == 1)             //判断杆状态，若为抬起则开始计杆抬起的时间
            {
                gjs++;
                if (gjs == 3)
                {
                    gjs = 0;
                    dd.gzt = 0;  //  根据单片机的程序3秒后杆自动落下，所以三秒后将杆标志位落下状态
                }
            }
         
            label22.Text = Convert.ToString(11 - dd.cw);   //更改剩余车辆的数字
            if (gd == ztsj)
            {
               
                timer2.Enabled = true;
                gd = 0;
            }
            label13.Text = DateTime.Now.ToString("20yy年MM月dd日 HH:mm:ss");
            
        }
/// <summary>
/// ////////////////////////////////////锁定界面按钮//////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button6_Click(object sender, EventArgs e)
        {
            if (panel1.Visible == true)             //若其他窗口是打开状态则关闭
            {
                panel1.Visible = false;
            }
            if (s1 != null && s1.Created)
            {
                s1.Close();
            }
            if (u != null && u.Created)
            {
                u.Close();
            }
            if (daohang != null && daohang.Created)
            {
                daohang.Close();
            }
            if (yhgl != null && yhgl.Created)
            {
                yhgl.Close();
            }
            if (bo != null && bo.Created)
            {
                bo.Close();
            }
            if (s != null && s.Created)
            {
                s.Close();
            }
            panel2.Visible = true;
            textBox1.Focus();
        }
/// <summary>
/// ///////////////////////////////实时监控按钮////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button2_Click(object sender, EventArgs e)
        {
          
           if (s != null && s.Created)
           {
               s.Focus();
               return;
           }
           else
           {
               s = new jiankong();
               
               s.Show();
           }
            
        }
        /// <summary>
        /// /////////////////////////////历史查询按钮////
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button5_Click(object sender, EventArgs e)
        {
            if (s1 != null && s1.Created)
            {
                s1.Focus();
                return;
            }
            s1 = new history();
            s1.Show();
            
        }

 /// <summary>
 /// //////////////////////管理员//////////
 /// </summary>
 /// <param name="sender"></param>
 /// <param name="e"></param>
       
        private void button4_Click(object sender, EventArgs e)
        {
            if (u != null && u.Created)
            {
                u.Focus();
                return;
            }
            
            u = new guanliyuan();
            
            u.Show();
               
        }
/// <summary>
/// ///////////////////////车位导航/
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button3_Click(object sender, EventArgs e)
        {
            if (daohang != null && daohang.Created)
            {
                daohang.Focus();
                return;
            }
            
            daohang = new go();
            
                daohang.Show();
           
           
        }
/// <summary>
/// /////////////////////窗口退出按钮////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void main_FormClosed(object sender, FormClosedEventArgs e)
        {
           Application.Exit();
        }
/// <summary>
/// ///////////////////帮助按钮///////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button10_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("使用说明.doc");
        }
/// <summary>
/// //////////////////////用户管理////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button8_Click(object sender, EventArgs e)
        {
            if (yhgl != null && yhgl.Created)
            {
                yhgl.Focus();
                return;
            }
            yhgl = new yonghuguanli();
            yhgl.Show();
        }
/// <summary>
/// //////////////////////信息通告/////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button9_Click(object sender, EventArgs e)
        {
            if (bo != null && bo.Created)
            {
                bo.Focus();
                return;
            }
            bo = new air();
            bo.Show();
        }

/// <summary>
/// ///////////////////////////主窗口关闭将管理员的下线时间写入数据库////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void main_FormClosing(object sender, FormClosingEventArgs e)
        {

            //string connString = "Data Source=IBLCTW;Database=car;Trusted_Connection=true;";   //定义连接数据库的字符串
            string connString = dd.sqlConnection;
            SqlConnection connection = new SqlConnection(connString);                            //创建连接对象
            SqlCommand chg = new SqlCommand();
            chg.Connection = connection;
            chg.CommandText = "UPDATE usr SET 下线时间='" + DateTime.Now + "' WHERE 用户名='" + dd.UserName + "' and 下线时间 is null";
            connection.Open();
            chg.ExecuteNonQuery();
        }

        /// <summary>
        /// ///////////////窗口启动后////////////////////////////
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void main_Shown(object sender, EventArgs e)
        {
            /////////串口默认设置////////////////////
            serialPort.PortName = "COM4";
            serialPort.BaudRate = 9600;
            serialPort.Parity = Parity.None;
            serialPort.DataBits = 8;
            serialPort.StopBits = StopBits.One;
            try
            {
                serialPort.Open();
                label15.Text = serialPort.PortName + "已连接";
                pictureBox1.Visible = true;
                pictureBox2.Visible = false;
            }
            catch
            {
                MessageBox.Show("默认串口无法连接，请手动设置！", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
            }
        }
/// <summary>
/// ///////////////////当窗口拖到屏幕顶部且鼠标离开窗体时时自动隐藏//////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void main_MouseEnter(object sender, EventArgs e)
        {
            if (state == 0) 
            { state = 1; 
              int t=this.Top;
              for (int i = t+1; i <= 0;i+=19 )
                  this.Top = i;
               Application.DoEvents();
               state = 0;
               
            }
        }

       private void main_MouseLeave(object sender, EventArgs e)
        {
            if ((Control.MousePosition.X > this.Left + 250) || (Control.MousePosition.X < this.Left + 9) || (Control.MousePosition.Y > 470))
           {
            if (this.Top <= 0 && state == 0 )
            {
                state = 1;
                int t = this.Top;
                for (int i = t; i > -this.Height + 2; i--)
                {
                    this.Top = i;
                    Application.DoEvents();
                }
                state = 0;
            }
           }
        }
/// <summary>
/// ////////////////////////解锁界面////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
       private void button11_Click(object sender, EventArgs e)
       {
           if (textBox1.Text.Trim().Equals(""))
           {
               textBox1.Text = "请输入密码";
               textBox1.Focus();
               label25.Visible = false;
           }
           else
           {
               if (textBox1.Text.Trim().Equals(dd.PassWord))        //如果输入的密码与当前登录的管理员密码相同
               {
                   textBox1.Text = "";
                   label25.Visible = false;
                   panel2.Visible = false;
               }
               else
               {
                   textBox1.Text = "";
                   textBox1.Focus();
                   label25.Visible = true;
               }
           }
       }
/// <summary>
/// //////////////////////鼠标置于串口连接图标上方显示串口连接情况///////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
       private void pictureBox1_MouseEnter(object sender, EventArgs e)
       {
           label15.Visible = true;
       }

       private void pictureBox1_MouseLeave(object sender, EventArgs e)
       {
           label15.Visible = false;
       }

       private void pictureBox2_MouseEnter(object sender, EventArgs e)
       {
           label15.Visible = true;
       }

       private void pictureBox2_MouseLeave(object sender, EventArgs e)
       {
           label15.Visible = false;
       }
/// <summary>
/// ///////////////////用于刷新信息时滚动的计时器///////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
       private void timer2_Tick(object sender, EventArgs e)
       {
           
           label14.Top-=4;
                                   
           if (label14.Top <= -panel4.Height)              //退出顶部则重新从底部进入      
           {
               
               switch (dd.gdcs)                 //循环滚动各条信息
               {
                   case 0: ztsj = 4; label14.Text = "温馨提示：" + "\n" + "(1)使用前请确认面板左下角的串口连接符号处于连接状态"; label14.ForeColor = Color.White; break;
                   case 1: ztsj = 4; label14.Text = "\n" + "(2)首次使用请打开帮助说明，参照说明书的步骤进行操作"; label14.ForeColor = Color.White; break;
                   case 2: ztsj = 4; label14.Text = "\n" + "(3)离开时请锁定界面防止非管理人员误操作"; label14.ForeColor = Color.White; break;
                   case 3: ztsj = 4; dd.gdcs = 0; label14.Text = "温馨提示：" + "\n" + "(1)使用前请确认面板左下角的串口连接符号处于连接状态"; label14.ForeColor = Color.White; break;
                   default: ztsj = 10; label14.Text = dd.fkxs; label14.ForeColor = Color.Yellow; dd.gdcs = dd.jzxs; break;            //收到反馈意见时显示反馈信息，然后接着上次的滚动信息继续显示下一条
               }
               dd.gdcs++;
               label14.Top = panel4.Height;
               
           }
           if (label14.Top == 0)                  //暂停两秒再滚动
           {
               timer2.Enabled = false;
           }
           Application.DoEvents();
       }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void label23_Click(object sender, EventArgs e)
        {

        }

        private void panel2_Paint(object sender, PaintEventArgs e)
        {

        }
    }
}
