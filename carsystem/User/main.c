#include "stm32f10x.h"
#include "carcontrol.h" 
#include "24l01.h"
#include "rc522.h"
#include <stdio.h>
#include "hw_exti.h"

//检查到黑线是1
#define SENSOR_4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)
#define SENSOR_3 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)
#define SENSOR_2 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
#define SENSOR_1 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)
#define SENSOR_0 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)

u8 flag=0;
u8 time_up=0;      //用于计时
extern u32 hw_jcq;
extern u8 hw_jsbz;


void Sensor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//sensor0、1、2、3
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}
void NVIC_Configuration(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure; 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 

	NVIC_InitStructure.NVIC_IRQChannel =EXTI15_10_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
}
void USART_Configuration(void)
{
    USART_InitTypeDef  USART_InitStructure;
	GPIO_InitTypeDef   	GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;//led   PA12
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;//led   PC13
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate=9600;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;

	USART_Init(USART1,&USART_InitStructure);
	//USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);
	USART_ClearFlag(USART1,USART_FLAG_TC);
}

///////////////////////////////////////////////////////////////////////
///*延时函数*/
///////////////////////////////////////////////////////////////////////
void delay_ms(u16 nms)
{
     //注意 delay_ms函数输入范围是1-1863
	 //所以最大延时为1.8秒

	 u32 temp;
	 SysTick->LOAD = 9000*nms;
	 SysTick->VAL=0X00;//清空计数器
	 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
	 do
	 {
	  temp=SysTick->CTRL;//读取当前倒计数值
	 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
	 SysTick->CTRL=0x00; //关闭计数器
	 SysTick->VAL =0X00; //清空计数器
}
void delay(int j)
{
	int i=0;
	while(j--)
		{
			i=12000;
			while(i--);
		}
}
void delay_us(u32 nus)//精确1us延时
{
	 u32 temp;
	 SysTick->LOAD = 9*nus;
	 SysTick->VAL=0X00;//清空计数器
	 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
	 do
	 {
	  temp=SysTick->CTRL;//读取当前倒计数值
	 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
	 
	 SysTick->CTRL=0x00; //关闭计数器
	 SysTick->VAL =0X00; //清空计数器
}

void steady(void)  //稳定小车通过程序
{
	while(SENSOR_3)
	{
		if(SENSOR_1)
		{
			TURN_RIGHT();
			while(SENSOR_1);
			MOVE_ON();
		}
	}
	delay_ms(10);
}	

int main()//24l01使用SPI1 RC522使用SPI2  总共 串口打印 红外传感器  NRF24L01模块  RC522模块  电机驱动 5个模块
{	
	u8 NUMBER=7;
	u8 receive[4]={0};	   //预约号  1#03  2#03
	u8 temp_position;    //用于暂时存储车的位置
	u8 flag_out=0; //出库标识
	u8 Rposition=1;//position为预约的车位Rposition是SENSOR_3计数的黑线
	u8 position=0;
	u8 location[32]="#1";
	u8 out_car[32]="6#0#0000#1@";
	
	Sensor_Init();		//红外探头
	USART_Configuration(); 		
	NVIC_Configuration();

	/////24l01配置
	NRF24L01_spi1();//无线要点 ：一定要注意配置好接收地址和发送地址，初始化的时候要调用到，同频道
	if(NRF24L01_Check()) printf("can`t find 24l01!");
	else printf("find 24l01 successfully!");
	NRF24L01_Init();//包括 RCC GPIO SPI1 CE=0 CS=1	
	NRF24L01_RX_Mode();//接收模式
		
	car_config();//电机初始化，主要是pwm初始化
	
 	RC522_Init(); //rc522初始化，复位，打开天线，ISO校验 
	
	EXTI_peizhi();  //红外对管配置，GPIO的配置在car config里面
	
	STOP();              //停车等待


	while(1)
	{	
//////////////////////////////////
//无线接收预约号,预约号格式：0#1#03 0区分码 1预约 03预约车位号，小车只有校验了预约号才会启动
//////////////////////////////////
		if(hw_jsbz==1)	
		{
			hw_jsbz=0;
			printf("字符串输出 %0.8X\r\n",hw_jcq);
//			GPIO_SetBits(GPIOA,GPIO_Pin_12);//LED测试
		}	
		if(NRF24L01_IRQ==0)//等待接收完成 已经配置成接受模式的情况下IRQ==0表示转换成发射模式
		{
			NRF24L01_RxPacket(receive);	
			printf("%s",receive); 
			temp_position=receive[3]-48;//ASCLL码转化
			switch(temp_position)
			{
				case 1: position=1;break; 
				case 2: position=2;break;
				case 3: position=5;break;
				case 4: position=6;break;
				case 5: position=7;break;
				case 6: position=8;break;
				case 7: position=9;break;
				
			}
			if((position>0)&&(position<=10))
			{	
//				GPIO_SetBits(GPIOC,GPIO_Pin_13);//LED测试			
			}	
		}
		
//////////////////////////////////
//1层6个车库
///////////////////////////////////	
        if(hw_jcq==0x00FFA25D)  //某一个固定的按键
		{	
			EXTI->PR=1<<11;  //清除LINE9上的中断标志位
			EXTI->IMR&=~(1<<11);//屏蔽line9上的中断  	
			GPIO_SetBits(GPIOC,GPIO_Pin_13);
			if(RC522_Handler()==0)			//读到卡停车
			{
				TIM_Cmd(TIM3, ENABLE);
				while(time_up==0)
				{
					if(SENSOR_1)
					{
//						GPIO_SetBits(GPIOA,GPIO_Pin_12);
						TURN_RIGHT();
						while(SENSOR_1);
						MOVE_ON();
					}
					if(SENSOR_2)//进行入库口检测的时候会导致小车有摇晃，主要是因为2号红外的影响
					{
//						GPIO_SetBits(GPIOC,GPIO_Pin_13);
						TURN_LEFT();
						while(SENSOR_2);
						MOVE_ON();
					}	
				}
				time_up=0;
				TIM_Cmd(TIM3, DISABLE);
				STOP();
				card_code(card_sequence);
				EXTI->IMR|=1<<11;//不屏蔽line11上的中断
				
				while(hw_jsbz==0);  //改成等待红外信号
				
				if(hw_jcq==0x00FF629D)//1号 2号出库
				{
					EXTI->PR=1<<11;  //清除LINE9上的中断标志位
					EXTI->IMR&=~(1<<11);//屏蔽line9上的中断  	
					
					MOVE_ON();
					while((!SENSOR_3))
					{
						
						if(SENSOR_1)
						{
							TURN_RIGHT();
							while(SENSOR_1);
							MOVE_ON();
						}
						if(SENSOR_2)//进行入库口检测的时候会导致小车有摇晃，主要是因为2号红外的影响
						{
							TURN_LEFT();
							while(SENSOR_2);
							MOVE_ON();
						}							
					}
					if(position==1) //1号出库算法
					{	
						out_car[0]=position+48;
						SEND_BUF(out_car);
						
						while(!SENSOR_0);
						while(SENSOR_0);
						delay_ms(100);	
						TURN_LEFT_90();
						while(!SENSOR_3);						
						while(!SENSOR_2);
						delay_ms(5);
						while(SENSOR_2);				
					}
					if(position==2) //2号出库算法
					{
						out_car[0]=position+48;
						SEND_BUF(out_car);
						
						TURN_LEFT();
						while(!SENSOR_0);
						delay_ms(5);
						while(SENSOR_0);
						delay_ms(5);
						TURN_LEFT_90();
						while(!SENSOR_1);
						MOVE_ON();						
					}
					MOVE_ON();
					flag_out=1;					
				}	
				
				if(hw_jcq==0x00FFE21D)//3号 4号出库
				{
					EXTI->PR=1<<11;  //清除LINE9上的中断标志位
					EXTI->IMR&=~(1<<11);//屏蔽line9上的中断  			
					MOVE_ON();
					while((!SENSOR_0))
					{
						
						if(SENSOR_1)
						{
							TURN_RIGHT();
							while(SENSOR_1);
							MOVE_ON();
						}
						if(SENSOR_2)//进行入库口检测的时候会导致小车有摇晃，主要是因为2号红外的影响
						{
							TURN_LEFT();
							while(SENSOR_2);
							MOVE_ON();
						}							
					}
					if(position==5)//3号出库算法
					{
						out_car[0]=3+48;
						SEND_BUF(out_car);
						
						TURN_RIGHT();
						while(!SENSOR_3);
						delay_ms(5);
						while(SENSOR_3);
						delay_ms(5);
						TURN_RIGHT_90();
						while(!SENSOR_2);
						MOVE_ON();
					}
					if(position==6) //4号出库算法
					{
						out_car[0]=4+48;
						SEND_BUF(out_car);
						
						while(!SENSOR_3);
						while(SENSOR_3);
						delay_ms(100);	
						TURN_RIGHT_90();
						while(!SENSOR_0);						
						while(!SENSOR_1);
						delay_ms(5);
						while(SENSOR_1);

					}
					MOVE_ON();
					flag_out=1;	
				}			
				if(hw_jcq==0x00FF22DD)//5 6 7号出库
				{
					EXTI->PR=1<<11;  //清除LINE9上的中断标志位
					EXTI->IMR&=~(1<<11);//屏蔽line9上的中断
					
					out_car[0]=temp_position+48; //出车位的字符
					SEND_BUF(out_car);	
					
					MOVE_ON();
					while((!SENSOR_3)&&(!SENSOR_0))
					{
						
						if(SENSOR_1)
						{
							TURN_RIGHT();
							while((SENSOR_1)&&(!SENSOR_3)&&(!SENSOR_0));
							MOVE_ON();
						}
						if(SENSOR_2)//进行入库口检测的时候会导致小车有摇晃，主要是因为2号红外的影响
						{
							TURN_LEFT();
							while((SENSOR_2)&&(!SENSOR_3)&&(!SENSOR_0));
							MOVE_ON();
						}							
					}
					while(!SENSOR_4);
					TURN_LEFT_BACK();
					while(!SENSOR_1);

					MOVE_ON();
					flag_out=1;	
				}					
			}	
//////////////////////////////////
//1层4个车库
///////////////////////////////////				 

			if((position>0)&&(position<7))  //0<x<7  
			{	
				MOVE_ON();//电机初始化，主要是pwm初始化
				SPEED(2);
				if(SENSOR_3)
				{
					delay_ms(5);
					if((Rposition==3)||(Rposition==4))//这几个正好是小车的4个大拐角
					{	
						TURN_LEFT();
						while(!SENSOR_0);
						delay_ms(5);
						while(SENSOR_0);
						delay_ms(5);
						TURN_LEFT_90();
						while(!SENSOR_1);
						MOVE_ON();	
						Rposition++;
					}			
					else
					{
						if(Rposition==position)//该函数是入库函数
						{
							TURN_LEFT();
							while(!SENSOR_0);
							delay_ms(5);
							while(SENSOR_0);
							delay_ms(5);
							TURN_LEFT_90();
							while(!SENSOR_1);
							MOVE_ON();						
						}
						else
						{
							GPIO_ResetBits(GPIOA,GPIO_Pin_12);
							MOVE_ON();
							while(SENSOR_3);
							while(!SENSOR_4)
							{
								if((SENSOR_1))
								{					
									TURN_RIGHT();
									while(SENSOR_1);
									MOVE_ON();
								}
								if((SENSOR_2))
								{					
									TURN_LEFT();
									while(SENSOR_2);
									MOVE_ON();
								}						
							}	
							Rposition++;
						}	
				   }	
				}
//	//////////////////////////////////
//	//直行算法
//	///////////////////////////////////	
				if((SENSOR_1))
				{
					TURN_RIGHT();
					while(SENSOR_1);
					MOVE_ON();
				}
				if((SENSOR_2))
				{
					TURN_LEFT();
					while(SENSOR_2);
					MOVE_ON();
				}
				
			}
			
//////////////////////////////////
////2层1 2 3库
///////////////////////////////////		
			
			if((position>=7)&&(position<10))
			{	
				
				MOVE_ON();
				SPEED(2);
				if(SENSOR_3)
				{	
					location[1]=NUMBER+48;
					SEND_BUF(location);		//发送定位信息	0#01
					delay_ms(5);
					if(NUMBER==position)//该函数是入库出库函数
					{	
//						GPIO_SetBits(GPIOC,GPIO_Pin_13);	
						TURN_LEFT();
						while(!SENSOR_0);
						delay_ms(10);
						while(SENSOR_0);
						delay_ms(5);
						TURN_LEFT_90();
						while(!SENSOR_1);
						MOVE_ON();						
						SPEED(2);

					}
					else
					{	
//						GPIO_SetBits(GPIOA,GPIO_Pin_12);
						MOVE_ON();
						while(SENSOR_3);
						while(!SENSOR_4)
						{
							if((SENSOR_1)&&(!SENSOR_3)&&(!SENSOR_0))
							{					
								TURN_RIGHT();
								while(SENSOR_1);
								MOVE_ON();
							}
							if((SENSOR_2)&&(!SENSOR_3)&&(!SENSOR_0))
							{					
								TURN_LEFT();
								while(SENSOR_2);
								MOVE_ON();
							}						
						}							
						NUMBER=NUMBER+1;
					}	
				}
				
				MOVE_ON();
				if(SENSOR_1)
				{
					TURN_RIGHT();
					while(SENSOR_1);
					MOVE_ON();
				}
				if(SENSOR_2)//进行入库口检测的时候会导致小车有摇晃，主要是因为2号红外的影响
				{
					TURN_LEFT();
					while(SENSOR_2);
					MOVE_ON();
				}
			}
		
		}
			
//////////////////////////////////
//出库
///////////////////////////////////	
		if(flag_out)
		{	
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			if(RC522_Handler()==0)			//读到卡停车 读卡
			{
				STOP();				
				card_code(card_sequence);
				EXTI->IMR|=1<<11;//不屏蔽line11上的中断				
				while(1);
			}
			MOVE_ON();
			if((SENSOR_1)&&(!SENSOR_3)&&(!SENSOR_0))
			{
				TURN_RIGHT();
				while(SENSOR_1);
				MOVE_ON();
			}
			if((SENSOR_2)&&(!SENSOR_3)&&(!SENSOR_0))//进行入库口检测的时候会导致小车有摇晃，主要是因为2号红外的影响
			{
				TURN_LEFT();
				while(SENSOR_2);
				MOVE_ON();
			}
		}
	}
}
 
