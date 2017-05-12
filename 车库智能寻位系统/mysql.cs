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
    public class mysql
    {
        SqlConnection connection = new SqlConnection();
        

        public int i;
        /// <summary>
        /// ////////////数据库连接//////////////////
        /// </summary>
        /// <param name="s"></param>
        /// <param name="b"></param>

        public int SqlConnDb(string s, string b)
        {
            try
            {
                string connString = string.Format("Data Source='{0}';Database='{1}';Trusted_Connection=true;", s, b);
                connection.ConnectionString = connString;
                if (connection.State == ConnectionState.Closed)
                {
                    connection.Open();
                    //判断数据库中是否已经存在表，否则建表,因为跳进这里则说明数据库还没建立，所以说明三个表都未建立，因此不必再逐个表检查/////////////
                    string cha = "history";
                    string tableview = "select count(1) from sysobjects where name ='" + cha + "'";

                    SqlCommand cmd0 = new SqlCommand(tableview, connection);

                    int result = Convert.ToInt32(cmd0.ExecuteScalar());

                    if (result == 0)
                    {

                        string cmdText = @"CREATE TABLE history(
                                       车位号 int not null,
                                       车主名 varchar(10),
                                       停靠时刻 datetime,
                                        离开时刻 datetime ,
                                        占用时间 int )";
                        SqlCommand cmd = new SqlCommand(cmdText, connection);
                        cmd.ExecuteNonQuery();

                        string cmdText1 = @"create table usr(
                                       用户名 varchar(50) not null,
                                       密码 varchar(50) not null,
                                       登录时间 datetime,
                                       下线时间 datetime)";
                        SqlCommand cmd1 = new SqlCommand(cmdText1, connection);
                        cmd1.ExecuteNonQuery();

                        string u = "iblctw", p = "123456";
                        string sql = string.Format("insert into usr(用户名,密码)" + "values('{0}','{1}')", u, p);
                        SqlCommand cmd2 = new SqlCommand(sql, connection);
                        //   connection.Open();
                        cmd2.ExecuteNonQuery();

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
                    return i = 1;

                }

            }
            catch
            {

                MessageBox.Show("数据库连接失败，请检查电脑设置！");
                return i = 0;
            }
            return i = 0;
        }
        /// <summary>
        /// //////////////////////////登录密码比对/////////////////////////
        /// </summary>
        /// <param name="u"></param>
        /// <param name="p"></param>
        /// <returns></returns>
        public int sqlchaxun(string u, string p)
        {
            string str = "select count(*) from usr where 用户名='" + u + "' and 密码='" + p + "'";    //查询数据库是否存在同时满足用户名和密码的数据
            SqlCommand cmd = new SqlCommand(str, connection);//创建命令对象;

            int n = (int)cmd.ExecuteScalar();
            return n;
        }


    }
}
