#include "stm32f10x.h"
#include "carcontrol.h" 
#include "24l01.h"
#include "rc522.h"
#include <stdio.h>
#include "hw_exti.h"

//��鵽������1
#define SENSOR_4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)
#define SENSOR_3 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)
#define SENSOR_2 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
#define SENSOR_1 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)
#define SENSOR_0 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)

u8 flag=0;
u8 time_up=0;      //���ڼ�ʱ
extern u32 hw_jcq;
extern u8 hw_jsbz;


void Sensor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//sensor0��1��2��3
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
///*��ʱ����*/
///////////////////////////////////////////////////////////////////////
void delay_ms(u16 nms)
{
     //ע�� delay_ms�������뷶Χ��1-1863
	 //���������ʱΪ1.8��

	 u32 temp;
	 SysTick->LOAD = 9000*nms;
	 SysTick->VAL=0X00;//��ռ�����
	 SysTick->CTRL=0X01;//ʹ�ܣ����������޶����������ⲿʱ��Դ
	 do
	 {
	  temp=SysTick->CTRL;//��ȡ��ǰ������ֵ
	 }while((temp&0x01)&&(!(temp&(1<<16))));//�ȴ�ʱ�䵽��
	 SysTick->CTRL=0x00; //�رռ�����
	 SysTick->VAL =0X00; //��ռ�����
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
void delay_us(u32 nus)//��ȷ1us��ʱ
{
	 u32 temp;
	 SysTick->LOAD = 9*nus;
	 SysTick->VAL=0X00;//��ռ�����
	 SysTick->CTRL=0X01;//ʹ�ܣ����������޶����������ⲿʱ��Դ
	 do
	 {
	  temp=SysTick->CTRL;//��ȡ��ǰ������ֵ
	 }while((temp&0x01)&&(!(temp&(1<<16))));//�ȴ�ʱ�䵽��
	 
	 SysTick->CTRL=0x00; //�رռ�����
	 SysTick->VAL =0X00; //��ռ�����
}

void steady(void)  //�ȶ�С��ͨ������
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

int main()//24l01ʹ��SPI1 RC522ʹ��SPI2  �ܹ� ���ڴ�ӡ ���⴫����  NRF24L01ģ��  RC522ģ��  ������� 5��ģ��
{	
	u8 NUMBER=7;
	u8 receive[4]={0};	   //ԤԼ��  1#03  2#03
	u8 temp_position;    //������ʱ�洢����λ��
	u8 flag_out=0; //�����ʶ
	u8 Rposition=1;//positionΪԤԼ�ĳ�λ�RRposition��SENSOR_3�����ĺ���
	u8 position=0;
	u8 location[32]="#1";
	u8 out_car[32]="6#0#0000#1@";
	
	Sensor_Init();		//����̽ͷ
	USART_Configuration(); 		
	NVIC_Configuration();

	/////24l01����
	NRF24L01_spi1();//����Ҫ�� ��һ��Ҫע�����úý��յ�ַ�ͷ��͵�ַ����ʼ����ʱ��Ҫ���õ���ͬƵ��
	if(NRF24L01_Check()) printf("can`t find 24l01!");
	else printf("find 24l01 successfully!");
	NRF24L01_Init();//���� RCC GPIO SPI1 CE=0 CS=1	
	NRF24L01_RX_Mode();//����ģʽ
		
	car_config();//�����ʼ������Ҫ��pwm��ʼ��
	
 	RC522_Init(); //rc522��ʼ������λ�������ߣ�ISOУ�� 
	
	EXTI_peizhi();  //����Թ����ã�GPIO��������car config����
	
	STOP();              //ͣ���ȴ�


	while(1)
	{	
//////////////////////////////////
//���߽���ԤԼ��,ԤԼ�Ÿ�ʽ��0#1#03 0������ 1ԤԼ 03ԤԼ��λ�ţ�С��ֻ��У����ԤԼ�ŲŻ�����
//////////////////////////////////
		if(hw_jsbz==1)	
		{
			hw_jsbz=0;
			printf("�ַ������ %0.8X\r\n",hw_jcq);
//			GPIO_SetBits(GPIOA,GPIO_Pin_12);//LED����
		}	
		if(NRF24L01_IRQ==0)//�ȴ�������� �Ѿ����óɽ���ģʽ�������IRQ==0��ʾת���ɷ���ģʽ
		{
			NRF24L01_RxPacket(receive);	
			printf("%s",receive); 
			temp_position=receive[3]-48;//ASCLL��ת��
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
//				GPIO_SetBits(GPIOC,GPIO_Pin_13);//LED����			
			}	
		}
		
//////////////////////////////////
//1��6������
///////////////////////////////////	
        if(hw_jcq==0x00FFA25D)  //ĳһ���̶��İ���
		{	
			EXTI->PR=1<<11;  //���LINE9�ϵ��жϱ�־λ
			EXTI->IMR&=~(1<<11);//����line9�ϵ��ж�  	
			GPIO_SetBits(GPIOC,GPIO_Pin_13);
			if(RC522_Handler()==0)			//������ͣ��
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
					if(SENSOR_2)//�������ڼ���ʱ��ᵼ��С����ҡ�Σ���Ҫ����Ϊ2�ź����Ӱ��
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
				EXTI->IMR|=1<<11;//������line11�ϵ��ж�
				
				while(hw_jsbz==0);  //�ĳɵȴ������ź�
				
				if(hw_jcq==0x00FF629D)//1�� 2�ų���
				{
					EXTI->PR=1<<11;  //���LINE9�ϵ��жϱ�־λ
					EXTI->IMR&=~(1<<11);//����line9�ϵ��ж�  	
					
					MOVE_ON();
					while((!SENSOR_3))
					{
						
						if(SENSOR_1)
						{
							TURN_RIGHT();
							while(SENSOR_1);
							MOVE_ON();
						}
						if(SENSOR_2)//�������ڼ���ʱ��ᵼ��С����ҡ�Σ���Ҫ����Ϊ2�ź����Ӱ��
						{
							TURN_LEFT();
							while(SENSOR_2);
							MOVE_ON();
						}							
					}
					if(position==1) //1�ų����㷨
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
					if(position==2) //2�ų����㷨
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
				
				if(hw_jcq==0x00FFE21D)//3�� 4�ų���
				{
					EXTI->PR=1<<11;  //���LINE9�ϵ��жϱ�־λ
					EXTI->IMR&=~(1<<11);//����line9�ϵ��ж�  			
					MOVE_ON();
					while((!SENSOR_0))
					{
						
						if(SENSOR_1)
						{
							TURN_RIGHT();
							while(SENSOR_1);
							MOVE_ON();
						}
						if(SENSOR_2)//�������ڼ���ʱ��ᵼ��С����ҡ�Σ���Ҫ����Ϊ2�ź����Ӱ��
						{
							TURN_LEFT();
							while(SENSOR_2);
							MOVE_ON();
						}							
					}
					if(position==5)//3�ų����㷨
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
					if(position==6) //4�ų����㷨
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
				if(hw_jcq==0x00FF22DD)//5 6 7�ų���
				{
					EXTI->PR=1<<11;  //���LINE9�ϵ��жϱ�־λ
					EXTI->IMR&=~(1<<11);//����line9�ϵ��ж�
					
					out_car[0]=temp_position+48; //����λ���ַ�
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
						if(SENSOR_2)//�������ڼ���ʱ��ᵼ��С����ҡ�Σ���Ҫ����Ϊ2�ź����Ӱ��
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
//1��4������
///////////////////////////////////				 

			if((position>0)&&(position<7))  //0<x<7  
			{	
				MOVE_ON();//�����ʼ������Ҫ��pwm��ʼ��
				SPEED(2);
				if(SENSOR_3)
				{
					delay_ms(5);
					if((Rposition==3)||(Rposition==4))//�⼸��������С����4����ս�
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
						if(Rposition==position)//�ú�������⺯��
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
//	//ֱ���㷨
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
////2��1 2 3��
///////////////////////////////////		
			
			if((position>=7)&&(position<10))
			{	
				
				MOVE_ON();
				SPEED(2);
				if(SENSOR_3)
				{	
					location[1]=NUMBER+48;
					SEND_BUF(location);		//���Ͷ�λ��Ϣ	0#01
					delay_ms(5);
					if(NUMBER==position)//�ú����������⺯��
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
				if(SENSOR_2)//�������ڼ���ʱ��ᵼ��С����ҡ�Σ���Ҫ����Ϊ2�ź����Ӱ��
				{
					TURN_LEFT();
					while(SENSOR_2);
					MOVE_ON();
				}
			}
		
		}
			
//////////////////////////////////
//����
///////////////////////////////////	
		if(flag_out)
		{	
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			if(RC522_Handler()==0)			//������ͣ�� ����
			{
				STOP();				
				card_code(card_sequence);
				EXTI->IMR|=1<<11;//������line11�ϵ��ж�				
				while(1);
			}
			MOVE_ON();
			if((SENSOR_1)&&(!SENSOR_3)&&(!SENSOR_0))
			{
				TURN_RIGHT();
				while(SENSOR_1);
				MOVE_ON();
			}
			if((SENSOR_2)&&(!SENSOR_3)&&(!SENSOR_0))//�������ڼ���ʱ��ᵼ��С����ҡ�Σ���Ҫ����Ϊ2�ź����Ӱ��
			{
				TURN_LEFT();
				while(SENSOR_2);
				MOVE_ON();
			}
		}
	}
}
 
