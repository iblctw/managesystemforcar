using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.IO;

namespace 车库智能寻位系统
{
    class carsocket
    {
        Encoding encoding = Encoding.GetEncoding("GB2312"); //解码器（可以用于汉字） 
        private Socket client;

        /// <summary>
        /// ////////传递socket连接参数/////////////
        /// </summary>
        /// <param name="ClientSocket"></param>
        public carsocket(Socket ClientSocket)
        {
            this.client = ClientSocket;
        }

        //数据处理接口        
        public void ClientServer()
        {
            //string connString = "Data Source=GHX;Database=car;Trusted_Connection=true;";   //定义连接数据库的字符串
            string connString = dd.sqlConnection;
            SqlConnection connection = new SqlConnection(connString);                            //创建连接对象
            
     
                string recvStr = "";
                
                byte[] recvBytes = new byte[1024];

                int bytes;

                bytes = client.Receive(recvBytes, recvBytes.Length, 0); //从客户端接受消息
                recvStr += encoding.GetString(recvBytes, 0, bytes);
                
            //////////////////////根据接收的数据的第一个字符判断客户端的需求，@为登录密码验证，#为用户余额，$为车库通告，%为反馈意见，^为修改密码，&为车位预约，*为获取已占用车位，（为取消预约，）为标记车位//////////////////
                if (recvBytes[0] == '@')
                {
                    recvStr = recvStr.Substring(1);
                    string panduan1 = "select count(*) from appusr where 用户名='" + recvStr + "'";  //判断数据库是否存在这个用户名
                    string panduan = "select 密码 from appusr where 用户名='" + recvStr + "'";   //根据客户端传过来的用户名查找数据库中对应的密码
                    SqlCommand cmd1 = new SqlCommand(panduan1, connection); 
                    SqlCommand cmd = new SqlCommand(panduan, connection); 
                    connection.Open();
                    int n = (int)cmd1.ExecuteScalar();
                    if (n >= 1)
                    {
                        SqlDataReader pas = cmd.ExecuteReader();
                        pas.Read();
                        string sendStr = pas["密码"].ToString();

                        byte[] bs = Encoding.ASCII.GetBytes(sendStr);

                        client.Send(bs, bs.Length, 0);  //返回密码给客户端
                    }
                    else
                    {
                        string nousr = "nouser ";                           //为了客户端程序方便取出非空值所以加了空格为字符结束标志
                        byte[] nu = Encoding.ASCII.GetBytes(nousr);
                        client.Send(nu, nu.Length, 0);
                    }
                }
                
            if (recvBytes[0] == '#')
                {
                    recvStr = recvStr.Substring(1);
                    string chayue = "select 账户余额 from appusr where 用户名='" + recvStr + "'";
                    SqlCommand cmd = new SqlCommand(chayue, connection);
                    connection.Open();
                    SqlDataReader yue = cmd.ExecuteReader();
                    yue.Read();
                    string sendStr = yue["账户余额"].ToString();
                    sendStr = sendStr + " ";                        //为了客户端程序方便取出非空值所以加了空格为字符结束标志
                    byte[] bs = Encoding.ASCII.GetBytes(sendStr);

                    client.Send(bs, bs.Length, 0);  //返回密码给客户端
                }
                
            if (recvBytes[0] == '$')
                {
                    string mess = dd.newmessege + "#" + dd.oldmessege+" ";  //新通告和历史通告合成一个字符串并用#区分
                    byte[] bs = encoding.GetBytes(mess);
                    
                    client.Send(bs, bs.Length, 0);  
                    
                }
                
            if (recvBytes[0] == '%')
                {
                    string fankui = "success ";
                    byte[] bs = encoding.GetBytes(fankui);
                    client.Send(bs, bs.Length, 0);
                    recvStr = recvStr.Substring(1);
                
                /////////////////将反馈的意见写入本地txt文件////////////////////////////
                    StreamWriter sw = new StreamWriter("fankui.txt", true, System.Text.Encoding.Unicode);
                    sw.WriteLine(recvStr);
                    sw.WriteLine("发自" + DateTime.Now.ToString("20yy年MM月dd日 HH:mm:ss"));
                    sw.WriteLine("******************");
                    sw.Close();
                  
                ///////////////////////显示在主面板上////////////////
                    main fkxx = new main();
                    dd.jzxs = dd.gdcs-1;            //记住滚动到的条数
                    dd.gdcs = 4;
                    if (recvStr.Length > 39)        //若消息字数超过一个版面则后面的字用省略号代替
                    {
                        dd.fkxs = "新消息：\n" + "  " + recvStr.Substring(0, 33) + "......";

                    }
                    else
                    {
                        dd.fkxs = "新消息：\n" + "  " + recvStr;
                    }
                      
                }
                
            if (recvBytes[0] == '^')
                {
                    recvStr = recvStr.Substring(1);
                    string[] passg;
                    passg = recvStr.Split('#');
                    string panduan1 = "select count(*) from appusr where 用户名='" + passg[0] + "'";  //判断数据库是否存在这个用户名
                    string panduan = "UPDATE appusr set 密码='"+passg[1]+"' where 用户名='" + passg[0] + "'";   //根据客户端传过来的用户名修改数据库中对应的密码
                    SqlCommand cmd1 = new SqlCommand(panduan1, connection);
                    connection.Open();
                    int n = (int)cmd1.ExecuteScalar();
                    if (n >= 1)
                    {
                        SqlCommand cmd = new SqlCommand(panduan, connection);
                        cmd.ExecuteNonQuery();
                        string xg = "success ";
                        byte[] bs = Encoding.ASCII.GetBytes(xg);

                        client.Send(bs, bs.Length, 0); 
                    }
                    else
                    {
                        string nousr = "nouser ";                           //为了客户端程序方便取出非空值所以加了空格为字符结束标志
                        byte[] nu = Encoding.ASCII.GetBytes(nousr);
                        client.Send(nu, nu.Length, 0);
                    }
                }

                
            if (recvBytes[0] == '&')
                {
                    recvStr = recvStr.Substring(1);
                    string[] yudh;
                    yudh = recvStr.Split('#');
                    
                    string ye = "select 账户余额 from appusr where 用户名='" + yudh[0] + "'";    //取出数据库中的余额数
                    SqlCommand cmd1 = new SqlCommand(ye, connection);
                    connection.Open();
                    SqlDataReader yue = cmd1.ExecuteReader();
                    yue.Read();
                    int yuer = (int)yue["账户余额"];

                    if (yuer > 0)                            //若账户余额不足不允许选择车位，且向客户端发出提醒
                    {

                        dd.xuanwei[Convert.ToInt32(yudh[1])] = yudh[0];     //以预定车位号为下标将车主名存入数组

                        string fankui = "success ";
                        byte[] bs = encoding.GetBytes(fankui);
                        client.Send(bs, bs.Length, 0);
                    }
                    else
                    {
                        string fankui = "false ";
                        byte[] bs = encoding.GetBytes(fankui);
                        client.Send(bs, bs.Length, 0);
                    }
                }

            if (recvBytes[0] == '*')
            {
                recvStr = recvStr.Substring(1);
                byte[] bs=new byte[13];
               
                for (int i = 1; i < 13; i++)
            {
                if (dd.xuanwei[i] != null)         //将已选择的车位号返回发送给客户端
                {
                    if (dd.xuanwei[i].Equals(recvStr))         //若为自己选择的车位号则存到数组的第一个值中
                    {
                        bs[0] = (byte)(i);
                    }
                    bs[i] =(byte)(i);
                
                }

                if (dd.cwzt[i] == 1)
                {
                    
                    bs[i] = (byte)(i);
                }
                
            }
             client.Send(bs, bs.Length, 0);
            }

            if (recvBytes[0] == '(')
            {
                recvStr = recvStr.Substring(1);
                
                int h=1;
                for (int i = 1; i < 13; i++)
                {
                    if (dd.xuanwei[i] != null)
                    {
                        if ((dd.xuanwei[i].Equals(recvStr)) && (dd.cwzt[i] == 0))     
                        {
                            dd.xuanwei[i] = null;
                            string quxiao = "success ";
                            byte[] bs = encoding.GetBytes(quxiao);
                            client.Send(bs, bs.Length, 0);
                            h = 0;
                            break;

                        }
                        else                           //若车未离开不允许取消选择
                            if (dd.xuanwei[i].Equals(recvStr))
                        {
                            string qx = "cannot ";
                            byte[] bs = encoding.GetBytes(qx);
                            client.Send(bs, bs.Length, 0);
                        }
                        
                    }

                }

                if (h == 1)
                {
                    string qx = "false ";
                    byte[] bs = encoding.GetBytes(qx);
                    client.Send(bs, bs.Length, 0);
                }
               
            }

            if (recvBytes[0] == ')')
            {
                recvStr = recvStr.Substring(1);
                string[] bjcw;
                bjcw = recvStr.Split('#');

       //         string kong="";           //用于表示车主名为空
                   

                ///////////////将标记该车位号的车主名写入数据库/////////////////////////////
                    SqlCommand chg = new SqlCommand();
                    chg.Connection = connection;
                    string zsj = string.Format("UPDATE history SET 车主名='{0}' WHERE 车位号='" + Convert.ToInt32(bjcw[1]) + "' and 离开时刻 is null and 车主名='"+""+"'", bjcw[0]);
                    chg.CommandText = zsj;
                    connection.Open();
                    int bj=chg.ExecuteNonQuery();
                    if (bj >= 1)
                    {
                        dd.xuanwei[Convert.ToInt32(bjcw[1])] = bjcw[0];     //以标记的车位号为下标将车主名存入数组
                        string fankui = "success ";
                        byte[] bs = encoding.GetBytes(fankui);
                        client.Send(bs, bs.Length, 0);
                    }
                    else
                    {
                        string fankui = "false ";
                        byte[] bs = encoding.GetBytes(fankui);
                        client.Send(bs, bs.Length, 0);
                    }
                    
              
            }

            client.Close();
        }
    }
}
