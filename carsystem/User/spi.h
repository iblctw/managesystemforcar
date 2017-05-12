#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"
void SPI1_Init(void);			 //初始化SPI1口
void SPI2_Init(void);			 //初始化SPI2口
void SPI1_SetSpeed(u8 SpeedSet);
u8 SPI1_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
u8 SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
		 
#endif
