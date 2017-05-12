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
    public partial class history : Form
    {
       
        public history()
        {
            InitializeComponent();
        }
        
        //SqlConnection conn = new SqlConnection("Data Source=IBLCTW;Database=car;Trusted_Connection=true;");  //实例化SqlConnection变量conn，连接数据库
        SqlConnection conn = new SqlConnection(dd.sqlConnection);
        SqlDataAdapter sda;
        DataSet ds;

        private void history_Load(object sender, EventArgs e)
        {
            
           
            try
            {
                sda = new SqlDataAdapter("select * from history", conn);  //创建一个SqlDataAdapter对象
                ds = new DataSet();   //创建一个DataSet对象
                sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
            }
            catch { }
        }
/// <summary>
/// ///////////////////////////////刷新按钮重绘表格///////////////////
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                sda = new SqlDataAdapter("select * from history", conn);  //创建一个SqlDataAdapter对象
                ds = new DataSet();   //创建一个DataSet对象
                sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
            }
            catch { }
        }

        
        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                string qian = comboBox1.Text + ":" + comboBox2.Text, hou = comboBox6.Text + ":" + comboBox5.Text;
                string chewei = textBox1.Text;
                if (chewei == "")
                {
                    sda = new SqlDataAdapter("select * from history where 停靠时刻>='" + dateTimePicker1.Value.ToString("yyyy-MM-dd '" + qian + "'") + "' and 停靠时刻<='" + dateTimePicker2.Value.ToString("yyyy-MM-dd '" + hou + "'") + "'", conn);  //创建一个SqlDataAdapter对象
                }
                else
                {
                    sda = new SqlDataAdapter("select * from history where 停靠时刻>='" + dateTimePicker1.Value.ToString("yyyy-MM-dd '" + qian + "'") + "' and 停靠时刻<='" + dateTimePicker2.Value.ToString("yyyy-MM-dd '" + hou + "'") + "' and 车位号='" + chewei + "'", conn);  //创建一个SqlDataAdapter对象
                }
                ds = new DataSet();   //创建一个DataSet对象
                sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
            }
            catch { }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("将删除所有符合条件的数据！", "", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button2) == DialogResult.OK)
            {
                try
                {
                    string qian = comboBox1.Text + ":" + comboBox2.Text, hou = comboBox6.Text + ":" + comboBox5.Text;
                    string chewei = textBox1.Text;
                    if (chewei == "")
                    {
                        sda = new SqlDataAdapter("delete from history where 停靠时刻>='" + dateTimePicker1.Value.ToString("yyyy-MM-dd '" + qian + "'") + "' and 停靠时刻<='" + dateTimePicker2.Value.ToString("yyyy-MM-dd '" + hou + "'") + "'", conn);  //创建一个SqlDataAdapter对象
                    }
                    else
                    {
                        sda = new SqlDataAdapter("delete from history where 停靠时刻>='" + dateTimePicker1.Value.ToString("yyyy-MM-dd '" + qian + "'") + "' and 停靠时刻<='" + dateTimePicker2.Value.ToString("yyyy-MM-dd '" + hou + "'") + "' and 车位号='" + chewei + "'", conn);  //创建一个SqlDataAdapter对象
                    }
                    ds = new DataSet();   //创建一个DataSet对象
                    sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                    try
                    {
                        sda = new SqlDataAdapter("select * from history", conn);  //创建一个SqlDataAdapter对象
                        ds = new DataSet();   //创建一个DataSet对象
                        sda.Fill(ds);   //使用SqlDataAdapter对象的Fill方法填充DataSet
                        dataGridView1.DataSource = ds.Tables[0];   //设置HisdataGridView控件数据源
                    }
                    catch { }
                }
                catch { }
            }
        }

        private void dateTimePicker2_ValueChanged(object sender, EventArgs e)
        {

        }

       
    }
}
