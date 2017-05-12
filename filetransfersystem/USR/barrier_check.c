#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
#include "stdio.h"
#include "24l01.h"
#include "barrier_check.h"
#include "pbdata.h"

char distance;
/*
void RCC_Configuration(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
}
void GPIO_Configuration(void)
{
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_All;//设PA 所以引脚为输入 
	//GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_All;//设PB 所以引脚为输入 
	//GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING ;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;//设trig为PA8
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

} */
void TIM_Configuration(void)
{	
	
	TIM_TimeBaseInitTypeDef TIM_BaseStructure;
	TIM_BaseStructure.TIM_Prescaler=35;
	TIM_BaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_BaseStructure.TIM_Period=199;
	TIM_BaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_BaseStructure.TIM_RepetitionCounter=0;
	
	TIM_TimeBaseInit(TIM2,&TIM_BaseStructure);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);	

}
char BARRIER_Check(unsigned char num)
{
	distance=0;
	TRIG_L;
	delay_us(2);
	TRIG_H;		//10us trig信号
	delay_us(15);
	TRIG_L;
	delay_us(1);
	if(num==1)
	{
		while(ECHO_1==0);	
		TIM2_ON;	
		while(ECHO_1==1);   //返回信号结束							  
		TIM2_OFF;
	}

	if(num==2)
	{
		while(ECHO_2==0);	
		TIM2_ON;	
		while(ECHO_2==1);   //返回信号结束							  
		TIM2_OFF;
	}

	if(num==3)
	{
		while(ECHO_3==0);	
		TIM2_ON;	
		while(ECHO_3==1);   //返回信号结束							  
		TIM2_OFF;
	}

	if(num==4)
	{
		while(ECHO_4==0);		
		TIM2_ON;	
		while(ECHO_4==1);   //返回信号结束							  
		TIM2_OFF;
	}
		if(num==5)
	{
		while(ECHO_5==0);		
		TIM2_ON;	
		while(ECHO_5==1);   //返回信号结束							  
		TIM2_OFF;
	}
		if(num==6)
	{
		while(ECHO_6==0);		
		TIM2_ON;	
		while(ECHO_6==1);   //返回信号结束							  
		TIM2_OFF;
	}
	/*
	if(num==6)
	{
		while(ECHO_6==0);
		 //捕获返回信号
		//	  if(distance>50)
		//		 return 0;	 //测距太近无回波，直接返回0
		//	distance=0;		
		TIM2_ON;	
		while(ECHO_6==1);   //返回信号结束							  
		TIM2_OFF;
		if(distance>15)			
			return 1;//
		else
			return 0;//
	}

	if(num==7)
	{
		while(ECHO_7==0);
		 //捕获返回信号
		//	  if(distance>50)
		//		 return 0;	 //测距太近无回波，直接返回0
		//	distance=0;		
		TIM2_ON;	
		while(ECHO_7==1);   //返回信号结束							  
		TIM2_OFF;
		if(distance>15)			
			return 1;//
		else
			return 0;//
	}

	if(num==8)
	{
		while(ECHO_8==0);
		 //捕获返回信号
		//	  if(distance>50)
		//		 return 0;	 //测距太近无回波，直接返回0
		//	distance=0;		
		TIM2_ON;	
		while(ECHO_8==1);   //返回信号结束							  
		TIM2_OFF;
		if(distance>15)			
			return 1;//
		else
			return 0;//
	}

	if(num==9)
	{
		while(ECHO_9==0);
		 //捕获返回信号
		//	  if(distance>50)
		//		 return 0;	 //测距太近无回波，直接返回0
		//	distance=0;		
		TIM2_ON;	
		while(ECHO_9==1);   //返回信号结束							  
		TIM2_OFF;
		if(distance>15)			
			return 1;//
		else
			return 0;//
	} */
	if(distance>15)			
		return 1;//
	else
		return 0;
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=0)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);		
		//USART_SendData(USART1,'b');		
		distance++;			
	}	
}
