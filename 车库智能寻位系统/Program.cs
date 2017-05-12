using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;


namespace 车库智能寻位系统
{
    public class dd
    {
        public static string[] data;    //存放接收到的车位号、占用情况、卡号
        public static int[] cwzt = new int[13];    //静态存储每个车位的占用状态
        public static int gzt = 0;   //标志杆的当前状态
        public static string[] xuanwei=new string[13]; //存储选择某个车位号的车主名,从下标1开始存储，车位总数12
        public static int[] yysj = new int[13] {0,0,0,0,0,0,0,0,0,0,0,0,0};    //每个车位的预约计时
        public static string UserName;
        public static string PassWord;
        public static string d = "y";     //设置变量d使同一个车位号的信息显示不换行
        public static int cw = 0;   //已用车位数，用于主面板显示剩余床位
        public static string newmessege="欢迎使用\n车库智能寻位系统";  //存放车库新通告
        public static string oldmessege="版权所有\n桂林电子科技大学\n信息与通信学院\n通信网络实验室";  //存放车库历史通告
        public static int gdcs = 0;       //用于记录滚动次数,每次滚动改变文字内容
        public static int jzxs;          //当有反馈信息进来需要显示反馈的内容时记录上次滚动到第几条，显示完反馈信息后接着上次的滚动下一条
        public static string fkxs;     //用于存储反馈信息显示的字符串
        public static string sqlConnection;    //用于储存SQL数据库连接的信息

    }

    
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new login());
        }
    }
}
