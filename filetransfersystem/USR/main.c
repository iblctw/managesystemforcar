#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
#include "stdio.h"
#include "24l01.h"
#include "pbdata.h"

extern u8 USART1_buff[4];
u8 RX_FLAG=0;
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART_Configuration(void);

int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

int main(void)
{  
//   u8 flag=0;
   u8 wireless_buff[12];
//   u8 USART1_buff[6]={0};//ԤԼ�Ÿ�ʽ  0#1#03   "1#0#0196#1@"
	
   RCC_Configuration();	//ϵͳʱ�ӳ�ʼ��
   GPIO_Configuration();//�˿ڳ�ʼ��
   USART_Configuration();
   NVIC_Configuration();
   SPI_Configuration();

	NRF24L01_CE_L;   //�ȹر�оƬʹ��                                         
	NRF24L01_CSN_H;

	if(NRF24L01_Check()) printf("can`t find 24l01!");
	else printf("find 24l01 successfully!");
	NRF24L01_Init();	//�������λ����Ƶ��40
	NRF24L01_RX_Mode();//����Ϊ����ģʽ	 
	while(1)
   {	   	
		//NRF24L01_RX_Mode();//����Ϊ����ģʽ��Ҫ������������Ž�ѭ�������ܽ��յ���Ϣ
	   //��������ʱ�Ϳ��ԣ���֪��Ϊʲô��delay_ms(10);
		if(NRF24L01_IRQ==0)//�ȴ�������� �Ѿ����óɽ���ģʽ�������IRQ==0��ʾת���ɷ���ģʽ
		{
			if(NRF24L01_RxPacket(wireless_buff)==0)
			{
				USART_send(wireless_buff);
				delay_ms(500);		
			}
		}
		
		if(RX_FLAG)
		{
			NRF24L01_TX_Mode(); 	//����Ϊ����ģʽ
			NRF24L01_TxPacket(USART1_buff);
			NRF24L01_RX_Mode(); 	//����Ϊ����ģʽ
			RX_FLAG=0;
		}
			
	}	
}

void RCC_Configuration(void)
{
  SystemInit();//72m
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|
												 RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
}

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	// nrf24l01����
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;// CE PA4 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;// CS PC4 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU  ;   //IRQ PA1��������,MINI��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//���ù���SPI1�Ĺܽ�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3);

}

void NVIC_Configuration(void)
{
   	NVIC_InitTypeDef NVIC_InitStructure; 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
}

void USART_Configuration(void)
{
    USART_InitTypeDef  USART_InitStructure;

	USART_InitStructure.USART_BaudRate=9600;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;

	USART_Init(USART1,&USART_InitStructure);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);			   	
	USART_Cmd(USART1,ENABLE);
	USART_ClearFlag(USART1,USART_FLAG_TC);
}

