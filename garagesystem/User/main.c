#include "stm32f10x.h"
#include "pin_config.h"
#include <stdio.h>
#include "mfrc522.h"
#include "24l01.h"
#include "motor.h"
/*
unsigned char data1[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
//M1卡的某一块写为如下格式，则该块为钱包，可接收扣款和充值命令
//4字节金额（低字节在前）＋4字节金额取反＋4字节金额＋1字节块地址＋1字节块地址取反＋1字节块地址＋1字节块地址取反 
unsigned char  data2[4]  = {0x12,0,0,0};
unsigned char  DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 


unsigned char g_ucTempbuf[20];                        
*/

int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;

}
int main()
{   
	RCC_Configuration();
	SPI_Configuration();
	NVIC_Configuration();
	USART_Configuration();
	Device_Config();
	TIM_Configuration();
	

//	读卡器初始化
	RC522_NSS_0_H;	
	RC522_NSS_1_H;
	NRF24L01_CE_L;                                            
	NRF24L01_CSN_H;
	RC522_Init();
	
	NRF24L01_CE_L;    //关闭芯片使能                                        
	NRF24L01_CSN_H;
	
	if(NRF24L01_Check()) printf("can`t find 24l01!");
	else printf("find 24l01 successfully!");
	NRF24L01_Init();
	NRF24L01_RX_Mode();//配置为接收模式
	//TIM4_ON;
	GPIO_SetBits(GPIOD,GPIO_Pin_All);
//	while(1);
	
	while(1)
	{	
			RC522_0_Handler();//注意要是换板，mini要禁用其他公用spi1的外设，无线的irq是PA1，其他的是PC5，要修改管脚基础配置，还要修改24l01。h的irq
							//现在小车是80，上位机是40，不要随意修改		
			RC522_1_Handler();
	}
	
}



