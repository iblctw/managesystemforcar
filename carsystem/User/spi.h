#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"
void SPI1_Init(void);			 //��ʼ��SPI1��
void SPI2_Init(void);			 //��ʼ��SPI2��
void SPI1_SetSpeed(u8 SpeedSet);
u8 SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
u8 SPI2_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
		 
#endif
