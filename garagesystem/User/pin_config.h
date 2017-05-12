#ifndef _STM32_SPI_H
#define _STM32_SPI_H
#include "sys.h"

//定义spi的函数
void RCC_Configuration(void);
void TIM_Configuration(void);
void SPI_Configuration(void);
void SPI1_SetSpeed(u8 SpeedSet);
void NVIC_Configuration(void);
void USART_Configuration(void);
void Device_Config(void);
u8 SPIWriteByte(u8 TxData);

//定义spi的变量
#define MOSI_H                GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define MOSI_L                GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define SCK_H                 GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define SCK_L                 GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define READ_MISO             GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)

// rc522 0 define
#define RC522_NSS_0_H         GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define RC522_NSS_0_L					GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define RC522_RST_0_H					GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define RC522_RST_0_L					GPIO_ResetBits(GPIOA,GPIO_Pin_1)
//rc522 1 define
#define RC522_NSS_1_H					GPIO_SetBits(GPIOA,GPIO_Pin_2)
#define RC522_NSS_1_L					GPIO_ResetBits(GPIOA,GPIO_Pin_2)
#define RC522_RST_1_H         GPIO_SetBits(GPIOA,GPIO_Pin_3)
#define RC522_RST_1_L         GPIO_ResetBits(GPIOA,GPIO_Pin_3)

//NRF24L01
#define NRF24L01_CE_L      	 	GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define NRF24L01_CE_H			    GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define NRF24L01_CSN_L			  GPIO_ResetBits(GPIOC,GPIO_Pin_4)
#define NRF24L01_CSN_H			  GPIO_SetBits(GPIOC,GPIO_Pin_4)

//电机定义
#define MOTOR0_IN1_L				GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define MOTOR0_IN1_H				GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define MOTOR0_IN2_L				GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define MOTOR0_IN2_H      			GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define MOTOR0_IN3_L				GPIO_ResetBits(GPIOB,GPIO_Pin_7)
#define MOTOR0_IN3_H				GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define MOTOR0_IN4_L				GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define MOTOR0_IN4_H				GPIO_SetBits(GPIOB,GPIO_Pin_8)

#define MOTOR1_IN1_L				GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define MOTOR1_IN1_H				GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define MOTOR1_IN2_L				GPIO_ResetBits(GPIOB,GPIO_Pin_10)
#define MOTOR1_IN2_H				GPIO_SetBits(GPIOB,GPIO_Pin_10)
#define MOTOR1_IN3_L				GPIO_ResetBits(GPIOB,GPIO_Pin_11)
#define MOTOR1_IN3_H				GPIO_SetBits(GPIOB,GPIO_Pin_11)
#define MOTOR1_IN4_L				GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define MOTOR1_IN4_H				GPIO_SetBits(GPIOB,GPIO_Pin_12)

//升降机
#define elevator_IN1_L				GPIO_ResetBits(GPIOE,GPIO_Pin_0)
#define elevator_IN1_H				GPIO_SetBits(GPIOE,GPIO_Pin_0)
#define elevator_IN2_L				GPIO_ResetBits(GPIOE,GPIO_Pin_1)
#define elevator_IN2_H				GPIO_SetBits(GPIOE,GPIO_Pin_1)
#define elevator_IN3_L				GPIO_ResetBits(GPIOE,GPIO_Pin_2)
#define elevator_IN3_H				GPIO_SetBits(GPIOE,GPIO_Pin_2)
#define elevator_IN4_L				GPIO_ResetBits(GPIOE,GPIO_Pin_3)
#define elevator_IN4_H				GPIO_SetBits(GPIOE,GPIO_Pin_3)

//LED定义
#define LED0_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_0)
#define LED0_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_0)
#define LED1_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_1)
#define LED1_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_1)
#define LED2_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_2)
#define LED2_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_2)
#define LED3_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_3)
#define LED3_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_3)
#define LED4_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_4)
#define LED4_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_4)
#define LED5_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_5)
#define LED5_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_5)
#define LED6_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_6)
#define LED6_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_6)
#define LED7_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_7)
#define LED7_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_7)
#define LED8_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_8)
#define LED8_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_8)
#define LED9_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_9)
#define LED9_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_9)
#define LED10_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_10)
#define LED10_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_10)
#define LED11_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_11)
#define LED11_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_11)
#define LED12_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_12)
#define LED12_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_12)
#define LED13_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_13)
#define LED13_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_13)
#define LED14_ON							GPIO_ResetBits(GPIOD,GPIO_Pin_14)
#define LED14_OFF 						GPIO_SetBits(GPIOD,GPIO_Pin_14)
//定时器开关
#define TIM2_ON  TIM_Cmd(TIM2,ENABLE)
#define TIM3_ON  TIM_Cmd(TIM3,ENABLE)
#define TIM4_ON  TIM_Cmd(TIM4,ENABLE)

#define TIM2_OFF	TIM_Cmd(TIM2,DIASBLE)
#define TIM3_OFF	TIM_Cmd(TIM3,DISABLE)
#define TIM4_OFF	TIM_Cmd(TIM4,DISABLE)
#endif
