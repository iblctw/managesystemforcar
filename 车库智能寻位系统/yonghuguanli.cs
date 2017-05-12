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
    public partial class yonghuguanli : Form
    {
        public yonghuguanli()
        {
            InitializeComponent();
        }
       
        //SqlConnection conn = new SqlConnection("Data Source=IBLCTW;Database=car;Trusted_Connection=true;");  //实例化SqlConnection变量conn，连接数据库
        SqlConnection conn = new SqlConnection(dd.sqlConnection);
        SqlDataAdapter sda;
        DataSet ds;

        private void yonghuguanli_Load(object sender, EventArgs e)
        {
            
            
            try
            {
                sda = new SqlDataAdapter("select * from appusr", conn);  //创建一个SqlDataAdapter对象
                ds = new DataSet();   //创建一个DataSet对象
                sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
                for (int i = 0; i < dataGridView1.RowCount-1; i++)          //隐藏密码显示
                {
                    dataGridView1.Rows[i].Cells[2].Value = "******";
                }
            }
            catch { }
        }
/// <summary>
/// /////////////////////////注册/////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button1_Click(object sender, EventArgs e)
        {
            // string connString = "Data Source=IBLCTW;Database=car;Trusted_Connection=true;";
            string connString = dd.sqlConnection;
            SqlConnection connection = new SqlConnection(connString);

            string kahao = textBox5.Text;
            string u = textBox1.Text;
            string m = textBox2.Text;
            int mrye=0;                                  //注册时默认余额0元
            if ((u == "") || (m == "")||(kahao==""))
            {
                MessageBox.Show("请填完所有表格！");
            }
            else
            {
                try
                {
                    string sql = string.Format("insert into appusr(卡号,用户名,密码,账户余额)" + "values('{0}','{1}','{2}','{3}')",kahao, u, m,mrye);

                    connection.Open();
                    SqlCommand cmd = new SqlCommand(sql, connection);
                    cmd.ExecuteNonQuery();
                    MessageBox.Show("注册成功！");

                    /////////////////////刷新表格数据////////////////////////
                    try
                    {
                        sda = new SqlDataAdapter("select * from appusr", conn);  //创建一个SqlDataAdapter对象
                        ds = new DataSet();   //创建一个DataSet对象
                        sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                        dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
                        for (int i = 0; i < dataGridView1.RowCount-1; i++)
                        {
                            dataGridView1.Rows[i].Cells[2].Value = "******";
                        }
                    }
                    catch { }
                }
                catch
                {
                    textBox1.Text = "用户名已存在";
                    textBox1.ForeColor =Color.Red;
                  //  MessageBox.Show("用户名已存在");
                }
            }
        }
 /// <summary>
 /// /////////////鼠标点击编辑框则自动显示刷出的卡号
 /// </summary>
 /// <param name="sender"></param>
 /// <param name="e"></param>
      
        private void textBox5_MouseDown(object sender, MouseEventArgs e)
        {
            if (dd.data != null)
            {
                this.Activate();
                textBox5.Text = dd.data[2];
            }
        }
/// <summary>
/// //////////////////账户余额充值
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button2_Click(object sender, EventArgs e)
        {
            string ckh = textBox3.Text;       //获取输入的卡号

            if (ckh == "")
            {
                MessageBox.Show("请输入卡号！");
            }
            else
            {
                string cye = textBox4.Text;
                if (cye == "")
                {
                    MessageBox.Show("请输入金额数！");
                }
                else
                {
                    int cyein = Convert.ToInt32(cye);    //获取输入的金额数
                    string ye = "select 账户余额 from appusr where 卡号='" + ckh + "'";    //取出数据库中的余额数
                    SqlCommand cmd1 = new SqlCommand(ye, conn);
                    conn.Open();
                    SqlDataReader yue = cmd1.ExecuteReader();
                    yue.Read();
                    int yuer = (int)yue["账户余额"];
                    yuer += cyein;                            //在原余额数上加上输入的金额数
                    yue.Close();

                    SqlCommand gye = new SqlCommand();
                    gye.Connection = conn;
                    gye.CommandText = "UPDATE appusr SET 账户余额='" + yuer + "' WHERE 卡号='" + ckh + "'";
                    gye.ExecuteNonQuery();

                    /////////////////////刷新表格数据////////////////////////
                    try
                    {
                        sda = new SqlDataAdapter("select * from appusr", conn);  //创建一个SqlDataAdapter对象
                        ds = new DataSet();   //创建一个DataSet对象
                        sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                        dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
                        for (int i = 0; i < dataGridView1.RowCount-1; i++)
                        {
                            dataGridView1.Rows[i].Cells[2].Value = "******";
                        }
                    }
                    catch { }
                }
            }
            conn.Close();
        }
/// <summary>
        /// //////////////////鼠标点击编辑框则自动显示刷出的卡号
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void textBox3_MouseDown(object sender, MouseEventArgs e)
        {
            if (dd.data != null)
            {
                this.Activate();
                textBox5.Text = dd.data[2];
            }
        }
/// <summary>
/// 删除用户
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button3_Click(object sender, EventArgs e)
        {
            // string connString = "Data Source=IBLCTW;Database=car;Trusted_Connection=true;";
            string connString = dd.sqlConnection;
            SqlConnection connection = new SqlConnection(connString);

            string kahao = textBox6.Text;
            if (kahao == "")
            {
                MessageBox.Show("请填写卡号！");
            }
            else
            {
                try
                {
                    string sql = string.Format("delete from appusr where 卡号='" + kahao + "'");

                    connection.Open();
                    SqlCommand cmd = new SqlCommand(sql, connection);
                    int aa=cmd.ExecuteNonQuery();
                    if (aa > 0)
                    {
                        MessageBox.Show("删除成功！");
                    }
                    else
                    {
                        MessageBox.Show("卡号不存在！");
                    }
                    /////////////////////刷新表格数据////////////////////////
                    try
                    {
                        sda = new SqlDataAdapter("select * from appusr", conn);  //创建一个SqlDataAdapter对象
                        ds = new DataSet();   //创建一个DataSet对象
                        sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                        dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
                        for (int i = 0; i < dataGridView1.RowCount - 1; i++)
                        {
                            dataGridView1.Rows[i].Cells[2].Value = "******";
                        }
                    }
                    catch { }
                }
                catch
                {
                   
                }
            }
        }

        private void textBox5_TextChanged(object sender, EventArgs e)
        {
            
        }

        private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

    }
}
