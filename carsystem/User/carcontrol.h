#ifndef __CARCONTROL_H
#define __CARCONTROL_H

#include "stm32f10x.h"

#define IN1_L GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define IN2_L GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define IN3_L GPIO_ResetBits(GPIOC,GPIO_Pin_15)
#define IN4_L GPIO_ResetBits(GPIOC,GPIO_Pin_14)

#define IN1_H GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define IN2_H GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define IN3_H GPIO_SetBits(GPIOC,GPIO_Pin_15)
#define IN4_H GPIO_SetBits(GPIOC,GPIO_Pin_14)

void car_config(void);
void START(void);
void steady_stop(void);
void STOP(void);
void GO_BACK(void);
void MOVE_ON(void);
void TURN_RIGHT_90(void);
void TURN_LEFT_90(void);
void fast_right(void);
void fast_left(void);
void TURN_RIGHT(void);
void TURN_LEFT(void);
void TURN_LEFT_BACK(void);
void SPEED(u8 LEVEL);

#endif

