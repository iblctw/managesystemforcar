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
using System.Runtime.InteropServices;
using System.Xml;



namespace 车库智能寻位系统
{
    public partial class login : Form
    {
        bool isMouseDown = false;                               //标记鼠标是否按下
        Point pointMouse = new Point();                         //记录鼠标按下时的坐标
        Point location;                                         //记录窗体坐标
        int angle;                                              //窗体抖动角度
        int radius = 20;                                        //窗体抖动半径

       //动画显示窗口声明//////////////
        [DllImport("user32.dll", EntryPoint = "AnimateWindow")]
        private static extern bool AnimateWindow(IntPtr handle, int ms, int flags);

        public login()
        {
            InitializeComponent();

            
        }

/// <summary>
/// //////////////////////////手动设置数据库完成后返回用户密码错误时的动作/////////////////////////
/// </summary>
        private void dou1()
        {
            int i = 1;
            location = this.Location;                       //记录窗体原始坐标
            angle = 0;                                      //将窗体抖动角置零

            while (i <= 2000)
            {

                i++;
                int x = location.X + (int)(radius * Math.Sin(angle * Math.PI / 180));//计算水平坐标
                int y = location.Y + (int)(radius * Math.Cos(angle * Math.PI / 180));//计算垂直坐标
                angle = (angle + 16) % 360;                         //增加窗体抖动角度
                this.Location = new Point(x, y);                    //设置窗体当前坐标

            }


            i = 1;

            this.Location = location;
            label6.Visible = true;
        }
/// <summary>
/// ////////////////////////////手动设置数据库成功后隐藏主窗口///////////////////
/// </summary>
        private void sht1()
        {
            //////////////////若勾选记住密码则将登录信息写入本地///////////
            if (checkBox1.Checked.Equals(true))
            {
                DES jiami = new DES();
                string jmmm = jiami.EncryptString(dd.PassWord);         //密码写入本地文件前加密

                XmlDocument xmldoc = new XmlDocument();
                xmldoc.Load("login.xml");                    //加载文件

                XmlNodeList nodeList = xmldoc.SelectSingleNode("login").ChildNodes;   //获取login节点的所有子节点
                foreach (XmlNode xn in nodeList)        //遍历所有节点
                {
                    XmlElement xe = (XmlElement)xn; //将子节点类型转换为XmlElement类型 
                    if (xe.GetAttribute("name") == dd.UserName)//如果本地已经存有这个用户名，则修改相关属性 
                    {
                        xe.SetAttribute("password", jmmm);
                        xe.SetAttribute("checked", "t");
                        xmldoc.Save("login.xml"); //保存其更改
                        goto pass;          //跳过添加节点的语句
                    }


                }
                XmlNode root = xmldoc.SelectSingleNode("login");   //查找login节点
                XmlElement xe1 = xmldoc.CreateElement("uers");     //添加一个uers节点
                xe1.SetAttribute("name", dd.UserName);              //设置登录名、密码、勾选状态的属性
                xe1.SetAttribute("password", jmmm);
                xe1.SetAttribute("checked", "t");
                root.AppendChild(xe1);
                xmldoc.Save("login.xml"); //保存其更改

            }
            else                      //若没有勾选记住密码则修改勾选属性
            {
                XmlDocument xmldoc = new XmlDocument();
                xmldoc.Load("login.xml");                    //加载文件

                XmlNodeList nodeList = xmldoc.SelectSingleNode("login").ChildNodes;   //获取login节点的所有子节点
                foreach (XmlNode xn in nodeList)        //遍历所有节点
                {
                    XmlElement xe = (XmlElement)xn; //将子节点类型转换为XmlElement类型 
                    if (xe.GetAttribute("name") == dd.UserName)//如果本地已经存有这个用户名，则只修改勾选属性 
                    {
                        xe.SetAttribute("checked", "f");
                        xmldoc.Save("login.xml"); //保存其更改
                        goto pass;          //跳出遍历
                    }


                }
            }

        pass:
            this.Hide();
        }
       

/// <summary>
/// ////////////////////////////////////////////////登录按钮//////////////////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button1_Click(object sender, EventArgs e)
        {
            
            int i = 1;
            dd.UserName = comboBox1.Text.Trim();//获取用户名
            dd.PassWord = textBox2.Text.Trim();//获取密码

            label6.Visible = false;         //首先隐藏所有提示信息，已便重新显示当前的提示
            label8.Visible = false;
            label9.Visible = false;
            ///////////////////为空则抖动窗口并提示///////////////////////
            if (dd.UserName == "")
            {
                location = this.Location;                       //记录窗体原始坐标
                angle = 0;                                      //将窗体抖动角置零

                while (i <= 2000)
                {

                    i++;
                    int x = location.X + (int)(radius * Math.Sin(angle * Math.PI / 180));//计算水平坐标
                    int y = location.Y + (int)(radius * Math.Cos(angle * Math.PI / 180));//计算垂直坐标
                    angle = (angle + 16) % 360;                         //增加窗体抖动角度
                    this.Location = new Point(x, y);                    //设置窗体当前坐标

                }


                i = 1;

                this.Location = location;
                label8.Visible = true;
            }
                
            else
            {
                SqlConnection connection = new SqlConnection();
              try
             {


                string connString = "Data Source=DESKTOP-G3H3VUM;Database=car;Trusted_Connection=true;"; 
               dd.sqlConnection= connString;
                connection.ConnectionString = connString;
                connection.Open();
                string lishi = "history";
                string yonhu = "appusr";
                string guanliyuan = "usr";
                ///////////////////////////判断数据库中是否已经存在表，否则建表/////////////

                string tableview = "select count(1) from sysobjects where name ='"+lishi+"'";
                string tableview1 = "select count(1) from sysobjects where name ='" + yonhu + "'";
                string tableview2 = "select count(1) from sysobjects where name ='" + guanliyuan + "'";
                SqlCommand cmd0 = new SqlCommand(tableview, connection);
                SqlCommand cmd5 = new SqlCommand(tableview1, connection);
                SqlCommand cmd6 = new SqlCommand(tableview2, connection);

                int result = Convert.ToInt32(cmd0.ExecuteScalar());
                int result1 = Convert.ToInt32(cmd5.ExecuteScalar());
                int result2 = Convert.ToInt32(cmd6.ExecuteScalar());

                if (result==0)
                {

                    string cmdText = @"CREATE TABLE history(
                                       车位号 int not null,
                                       车主名 varchar(10),
                                       停靠时刻 datetime,
                                       离开时刻 datetime ,
                                       占用时间 int )";
                    SqlCommand cmd1 = new SqlCommand(cmdText, connection);
                    cmd1.ExecuteNonQuery();
                }
                if(result2==0)
                {
                    string cmdText1 = @"create table usr(
                                       用户名 varchar(50) not null,
                                       密码 varchar(50) not null,
                                       登录时间 datetime,
                                       下线时间 datetime)";
                    SqlCommand cmd2 = new SqlCommand(cmdText1, connection);
                    cmd2.ExecuteNonQuery();

                    ////////////////初始设置默认用户名密码////////////////////////////////
                    string u = "ghx", p = "123456";
                    string sql = string.Format("insert into usr(用户名,密码)" + "values('{0}','{1}')", u, p);
                    SqlCommand cmd3 = new SqlCommand(sql, connection);
                    //   connection.Open();
                    cmd3.ExecuteNonQuery();
                }
                if(result1==0)
                {
                    string cmdText2 = @"create table appusr(
                                       卡号 nchar(10) not null,
                                       用户名 nchar(10) not null primary key,
                                       密码 nchar(10) not null,
                                       入库时间 datetime null,
                                       出库时间 datetime null,
                                       账户余额 int null)";
                    SqlCommand cmd4 = new SqlCommand(cmdText2, connection);
                    cmd4.ExecuteNonQuery();
                }
                    
               

                  ////////////将输入的用户名和密码在数据库中查询是否存在///////////////////

                string str = "select count(*) from usr where 用户名='" + dd.UserName + "'";
                string str1 = "select 密码 from usr where 用户名='" + dd.UserName + "'";    //取出数据库中对应用户名的密码
                SqlCommand cmd = new SqlCommand(str, connection);//创建命令对象;
                SqlCommand cmd7 = new SqlCommand(str1, connection);
                //  mysql ch = new mysql();

                //  int n =ch.sqlchaxun(UserName, PassWord);
                int n = (int)cmd.ExecuteScalar();

                if (n >= 1)                  //判断是否存在用户名                 
                {
                    SqlDataReader jzmm = cmd7.ExecuteReader();
                    jzmm.Read();
                    string mm = (string)jzmm["密码"];
                    jzmm.Close();

             
                    if (mm.Equals(dd.PassWord))                         //////密码正确将管理员登录信息写入数据库并开启主窗口
                    {
                        
                        SqlCommand chg = new SqlCommand();        //更新第一次注册的用户登录时间
                        chg.CommandText = "UPDATE usr SET 登录时间='" + DateTime.Now + "' WHERE 用户名='" + dd.UserName + "' and 登录时间 is null";
                        string sql = string.Format("insert into usr(用户名,密码,登录时间)" + "values('{0}','{1}','{2}')", dd.UserName, dd.PassWord, DateTime.Now);
                        SqlCommand cmd3 = new SqlCommand(sql, connection);
                        chg.Connection = connection;
                        int ya = chg.ExecuteNonQuery();
                        if (ya == 0)                   ///若不是第一次注册用户则添加一行记录他的登录时间
                        {
                            cmd3.ExecuteNonQuery();
                        }

                        //////////////////若勾选记住密码则将登录信息写入本地///////////
                        if (checkBox1.Checked.Equals(true))
                        {
                            DES jiami = new DES();
                            string jmmm = jiami.EncryptString(dd.PassWord);         //密码写入本地文件前加密

                            XmlDocument xmldoc = new XmlDocument();
                            xmldoc.Load("login.xml");                    //加载文件

                            XmlNodeList nodeList = xmldoc.SelectSingleNode("login").ChildNodes;   //获取login节点的所有子节点
                            foreach (XmlNode xn in nodeList)        //遍历所有节点
                            {
                                XmlElement xe = (XmlElement)xn; //将子节点类型转换为XmlElement类型 
                                if (xe.GetAttribute("name") == dd.UserName)//如果本地已经存有这个用户名，则修改相关属性 
                                {
                                    
                                    xe.SetAttribute("password", jmmm);
                                    xe.SetAttribute("checked", "t");
                                    xmldoc.Save("login.xml"); //保存其更改
                                    goto pass;          //跳过添加节点的语句
                                }


                            }
                            XmlNode root = xmldoc.SelectSingleNode("login");   //查找login节点
                            XmlElement xe1 = xmldoc.CreateElement("uers");     //添加一个uers节点
                            xe1.SetAttribute("name", dd.UserName);              //设置登录名、密码、勾选状态的属性
                            xe1.SetAttribute("password", jmmm);
                            xe1.SetAttribute("checked", "t");
                            root.AppendChild(xe1);
                            xmldoc.Save("login.xml"); //保存其更改

                        }
                        else                      //若没有勾选记住密码则修改勾选属性
                        {
                            XmlDocument xmldoc = new XmlDocument();
                            xmldoc.Load("login.xml");                    //加载文件

                            XmlNodeList nodeList = xmldoc.SelectSingleNode("login").ChildNodes;   //获取login节点的所有子节点
                            foreach (XmlNode xn in nodeList)        //遍历所有节点
                            {
                                XmlElement xe = (XmlElement)xn; //将子节点类型转换为XmlElement类型 
                                if (xe.GetAttribute("name") == dd.UserName)//如果本地已经存有这个用户名，则只修改勾选属性 
                                {
                                    xe.SetAttribute("checked", "f");
                                    xmldoc.Save("login.xml"); //保存其更改
                                    goto pass;          //跳出遍历
                                }


                            }
                        }

                    pass:
                        main start = new main();
                        start.Show();
                        this.Hide();
                    }
                    else                                       ///////////////密码错误导致窗口抖动//////////////////
                    {
                        location = this.Location;                       //记录窗体原始坐标
                        angle = 0;                                      //将窗体抖动角置零

                        while (i <= 2000)
                        {

                            i++;
                            int x = location.X + (int)(radius * Math.Sin(angle * Math.PI / 180));//计算水平坐标
                            int y = location.Y + (int)(radius * Math.Cos(angle * Math.PI / 180));//计算垂直坐标
                            angle = (angle + 16) % 360;                         //增加窗体抖动角度
                            this.Location = new Point(x, y);                    //设置窗体当前坐标

                        }


                        i = 1;

                        this.Location = location;
                        label6.Visible = true;


                    }
                }
                else                                       ///////////////用户名不存在导致窗口抖动//////////////////
                {
                    location = this.Location;                       //记录窗体原始坐标
                    angle = 0;                                      //将窗体抖动角置零

                    while (i <= 2000)
                    {

                        i++;
                        int x = location.X + (int)(radius * Math.Sin(angle * Math.PI / 180));//计算水平坐标
                        int y = location.Y + (int)(radius * Math.Cos(angle * Math.PI / 180));//计算垂直坐标
                        angle = (angle + 16) % 360;                         //增加窗体抖动角度
                        this.Location = new Point(x, y);                    //设置窗体当前坐标

                    }


                    i = 1;

                    this.Location = location;
                    label9.Visible = true;


                }
              }
                
           catch
           {


               setsql sq = new setsql();

               sq.dou += new setsql.chclose(this.dou1);
               sq.sht += new setsql.chclose(this.sht1);
               sq.Show();


            }
                
            } 
                 
     }
        
/// <summary>
/// ///////////////////////////当前日期时间////////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void timer1_Tick(object sender, EventArgs e)
        {
            label4.Text = DateTime.Now.ToString("HH:mm");
            label5.Text = DateTime.Now.ToString("yyyy年MM月dd日");
            label7.Text = DateTime.Now.ToString("ss");
        }
/// <summary>
/// ////////////////////////////////退出按钮//////////////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button2_Click(object sender, EventArgs e)
        {
            Application.Exit();    
        }
/// <summary>
/// ////////////////////////////////////////无边框拖动/////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void login_MouseDown(object sender, MouseEventArgs e)
        {
            pointMouse = e.Location;                            //记录鼠标按下时的坐标
            isMouseDown = true;                                 //记录鼠标已按下
        }

        private void login_MouseMove(object sender, MouseEventArgs e)
        {
            if (isMouseDown)
            {
                Point location = this.PointToScreen(e.Location);//记录鼠标屏幕坐标
                location.Offset(-pointMouse.X, -pointMouse.Y);  //将鼠标屏幕坐标转换成窗体坐标
                this.Location = location;                       //设置窗体坐标
                
            }
        }

        private void login_MouseUp(object sender, MouseEventArgs e)
        {
            isMouseDown = false;                                 //记录鼠标未按下
        }
/// <summary>
/// //////////////////////////////////////////窗口启动////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        
        
        private void login_Load(object sender, EventArgs e)
        {


            AnimateWindow(this.Handle, 150, 0x0010);     //实现从中间展开窗口动画

            label4.Text = DateTime.Now.ToString("HH:mm");
            label5.Text = DateTime.Now.ToString("yyyy年MM月dd日");
            label7.Text = DateTime.Now.ToString("ss");

          ////////////////////读取本地的登录信息//////////////////////////////////
            XmlDocument xmldoc = new XmlDocument();
            xmldoc.Load("login.xml");                    //加载文件
            xmldoc =new XmlDocument();
            xmldoc.Load("login.xml"); //加载xml文件
            XmlNode xn = xmldoc.SelectSingleNode("login");    //选择节点
            XmlNodeList xnl = xn.ChildNodes;        //获取该节点的子节点
            foreach (XmlNode xnf in xnl)          //遍历所有节点
            {
                XmlElement xe = (XmlElement)xnf;
                comboBox1.Items.Add(xe.GetAttribute("name"));         //将本地的登录名写入登陆框的可选项中
                comboBox1.Text = xe.GetAttribute("name");            //填充用户名
                if (xe.GetAttribute("checked")=="t")                  //判断各用户的勾选状态
                {
                    checkBox1.Checked = true;
                    DES jiemi = new DES();
                    textBox2.Text = jiemi.DecryptString(xe.GetAttribute("password"));   //自动填充解密后的密码
                }
                else
                {
                    checkBox1.Checked = false;
                    textBox2.Text = "";
                }
            }


        }
/// <summary>
/// /////////////////////登录名改变时重新加载登录信息///////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void comboBox1_TextChanged(object sender, EventArgs e)
        {
            XmlDocument xmldoc = new XmlDocument();
            xmldoc.Load("login.xml");                    //加载文件
            xmldoc = new XmlDocument();
            xmldoc.Load("login.xml"); //加载xml文件
            XmlNode xn = xmldoc.SelectSingleNode("login");    //选择节点
            XmlNodeList xnl = xn.ChildNodes;        //获取该节点的子节点
            foreach (XmlNode xnf in xnl)          //遍历所有节点
            {
                XmlElement xe = (XmlElement)xnf;
                if (xe.GetAttribute("name") == comboBox1.Text)          //判断节点属性是否等于当前选择的用户名
                {

                    if (xe.GetAttribute("checked") == "t")                  //判断当前用户的勾选状态
                    {
                        checkBox1.Checked = true;
                        DES jiemi = new DES();
                        textBox2.Text = jiemi.DecryptString(xe.GetAttribute("password"));   //自动填充解密后的密码

                    }
                    else
                    {
                        checkBox1.Checked = false;
                        textBox2.Text = "";

                    }
                }
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
       
       
    }
}
