using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data.SqlClient;

using System.Windows.Forms;

namespace 车库智能寻位系统
{
    public partial class setsql : Form
    {
        public delegate void chclose();   ////定义委托，实现数据库设置子窗口调用主窗口事件
        public event chclose dou;
        public event chclose sht;
       
        public setsql()
        {
            InitializeComponent();
        }

       
     
        private void button1_Click(object sender, EventArgs e)
        {
            
            
            string fu, shu;
            fu = textBox1.Text.Trim();
            shu = textBox2.Text.Trim();
            mysql cne = new mysql();
            cne.SqlConnDb(fu, shu);
            if (cne.i == 1)
            {
                login te = new login();

                int n = cne.sqlchaxun(dd.UserName, dd.PassWord);

                if (n >= 1)
                {
                    ////////////////向数据库写入当前管理员的登录时间
                    SqlConnection connection = new SqlConnection();
                    string connString = string.Format("Data Source='{0}';Database='{1}';Trusted_Connection=true;", fu, shu);
                    connection.ConnectionString = connString;
                    connection.Open();
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

                    main start = new main();
                    start.Show();
                    //       te.Hide();
                    sht();
                    this.Close();
                }
                else                                       ///////////////密码错误导致窗口抖动//////////////////
                {
                    //    MessageBox.Show("用户名或密码错误");
                    dou();
                    this.Close();
                }
            }
            this.Close();
            
            
    
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void setsql_Load(object sender, EventArgs e)
        {

        }
    }
}
