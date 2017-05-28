#include "stm32f10x.h"
#include "pin_config.h"
#include <stdio.h>
#include "mfrc522.h"
#include "24l01.h"
#include "motor.h"
/*
unsigned char data1[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
//M1����ĳһ��дΪ���¸�ʽ����ÿ�ΪǮ�����ɽ��տۿ�ͳ�ֵ����
//4�ֽڽ����ֽ���ǰ����4�ֽڽ��ȡ����4�ֽڽ�1�ֽڿ��ַ��1�ֽڿ��ַȡ����1�ֽڿ��ַ��1�ֽڿ��ַȡ�� 
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
	

//	��������ʼ��
	RC522_NSS_0_H;	
	RC522_NSS_1_H;
	NRF24L01_CE_L;                                            
	NRF24L01_CSN_H;
	RC522_Init();
	
	NRF24L01_CE_L;    //�ر�оƬʹ��                                        
	NRF24L01_CSN_H;
	
	if(NRF24L01_Check()) printf("can`t find 24l01!");
	else printf("find 24l01 successfully!");
	NRF24L01_Init();
	NRF24L01_RX_Mode();//����Ϊ����ģʽ
	//TIM4_ON;
	GPIO_SetBits(GPIOD,GPIO_Pin_All);
//	while(1);
	
	while(1)
	{	
			RC522_0_Handler();//ע��Ҫ�ǻ��壬miniҪ������������spi1�����裬���ߵ�irq��PA1����������PC5��Ҫ�޸ĹܽŻ������ã���Ҫ�޸�24l01��h��irq
							//����С����80����λ����40����Ҫ�����޸�		
			RC522_1_Handler();
	}
	
}



