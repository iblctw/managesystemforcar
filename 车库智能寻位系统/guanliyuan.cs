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
    public partial class guanliyuan : Form
    {
        public guanliyuan()
        {
            InitializeComponent();
        }

        // SqlConnection conn = new SqlConnection("Data Source=IBLCTW;Database=car;Trusted_Connection=true;");  //实例化SqlConnection变量conn，连接数据库
        SqlConnection conn = new SqlConnection(dd.sqlConnection);
        SqlDataAdapter sda;
        DataSet ds;

/// <summary>
/// ///////////////修改密码///////////////////////////////////////////////////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button1_Click(object sender, EventArgs e)
        {
            //string connString = "Data Source=IBLCTW;Database=car;Trusted_Connection=true;";
            string connString = dd.sqlConnection;
            SqlConnection connection = new SqlConnection(connString);

            string u = textBox1.Text;
            string m = textBox2.Text;
            
            int count;

            if ((u == "") || (m == ""))
            {
                MessageBox.Show("请确定已输入", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
            }
            else
            {
                try
                {
                    SqlCommand chg = new SqlCommand();
                    chg.Connection = connection;
                    string sql = string.Format("UPDATE usr SET 密码='{0}' WHERE 用户名='{1}'", m, u);
                    chg.CommandText = sql;
                    connection.Open();
                    count = chg.ExecuteNonQuery();
                    if (count > 0)
                    {
                        MessageBox.Show("密码修改成功！", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                        /////////////////////刷新表格数据////////////////////////
                        try
                        {
                            sda = new SqlDataAdapter("select * from usr", conn);  //创建一个SqlDataAdapter对象
                            ds = new DataSet();   //创建一个DataSet对象
                            sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                            dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
                            for (int i = 0; i < dataGridView1.RowCount-1; i++)                         //将密码隐藏
                            {
                                dataGridView1.Rows[i].Cells[1].Value = "******";
                            }
                        }
                        catch { }
                    }


                    else
                        MessageBox.Show("用户名不存在！", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                }
                catch
                {
                    MessageBox.Show("修改失败", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                }
            }
        }
/// <summary>
/// 添加管理员
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button2_Click(object sender, EventArgs e)
        {
            //string connString = "Data Source=IBLCTW;Database=car;Trusted_Connection=true;";
            string connString = dd.sqlConnection;
            SqlConnection connection = new SqlConnection(connString);

            string u = textBox1.Text;
            string m = textBox2.Text;
            if ((u == "") || (m == ""))
            {
                MessageBox.Show("请确定已输入", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
            }
            else
            {
                try
                {
                    string panduan1 = "select count(*) from usr where 用户名='" + u + "'";  //判断数据库是否存在这个用户名
                    string sql = string.Format("insert into usr(用户名,密码)" + "values('{0}','{1}')", u, m);

                    connection.Open();
                    SqlCommand cmd1 = new SqlCommand(panduan1, connection);
                    SqlCommand cmd = new SqlCommand(sql, connection);
                    
                    int n = (int)cmd1.ExecuteScalar();
                    if (n == 0)
                    {
                        cmd.ExecuteNonQuery();
                        MessageBox.Show("添加成功！", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);

                        /////////////////////刷新表格数据////////////////////////
                        try
                        {
                            sda = new SqlDataAdapter("select * from usr", conn);  //创建一个SqlDataAdapter对象
                            ds = new DataSet();   //创建一个DataSet对象
                            sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                            dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
                            for (int i = 0; i < dataGridView1.RowCount-1; i++)
                            {
                                dataGridView1.Rows[i].Cells[1].Value = "******";
                            }
                        }
                        catch { }
                    }
                    else
                    {
                        textBox1.Text = "用户名已存在";
                        textBox1.ForeColor = Color.Red;
                    }
                }
                catch
                {
                    MessageBox.Show("添加失败！", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                }
            }
        }
/// <summary>
/// ///////////////////////显示管理员数据库数据////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void user_Load(object sender, EventArgs e)
        {
            try
            {
                sda = new SqlDataAdapter("select * from usr", conn);  //创建一个SqlDataAdapter对象
                ds = new DataSet();   //创建一个DataSet对象
                sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
                for (int i = 0; i < dataGridView1.RowCount-1; i++)
                {
                    dataGridView1.Rows[i].Cells[1].Value = "******";
                }
            }
            catch { }
        }
/// <summary>
/// 删除管理员
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button3_Click(object sender, EventArgs e)
        {
            //string connString = "Data Source=IBLCTW;Database=car;Trusted_Connection=true;";
            string connString = dd.sqlConnection;
            SqlConnection connection = new SqlConnection(connString);

            string u = textBox1.Text;
          //  string m = textBox2.Text;
            if (u == "")
            {
                MessageBox.Show("请确定已输入", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
            }
            else
            {
                try
                {
                    string panduan1 = "select count(*) from usr where 用户名='" + u + "'";  //判断数据库是否存在这个用户名
                    string sql = string.Format("delete from usr where 用户名='" + u + "'");

                    connection.Open();
                    SqlCommand cmd1 = new SqlCommand(panduan1, connection);
                    SqlCommand cmd = new SqlCommand(sql, connection);

                    int n = (int)cmd1.ExecuteScalar();
                    if (n != 0)
                    {
                        cmd.ExecuteNonQuery();
                        MessageBox.Show("删除成功！", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);

                        /////////////////////刷新表格数据////////////////////////
                        try
                        {
                            sda = new SqlDataAdapter("select * from usr", conn);  //创建一个SqlDataAdapter对象
                            ds = new DataSet();   //创建一个DataSet对象
                            sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                            dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
                            for (int i = 0; i < dataGridView1.RowCount - 1; i++)
                            {
                                dataGridView1.Rows[i].Cells[1].Value = "******";
                            }
                        }
                        catch { }
                    }
                    else
                    {
                        textBox1.Text = "用户名不存在";
                        textBox1.ForeColor = Color.Red;
                    }
                }
                catch
                {
                    MessageBox.Show("添加失败！", "智能车库提醒您", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                }
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        
    }
}
