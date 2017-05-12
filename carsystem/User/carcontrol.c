#include "carcontrol.h"

extern void delay(int j);
extern void delay_ms(u16 nms);

void car_config(void)
{   
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_BaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;//hw
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
			
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;//in1 in2 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14|GPIO_Pin_15;//in3 in4
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_14|GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;//ENA ENB 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_3);
	
	/*********tim2 pwm 频率设置*************/
	
    TIM_BaseStructure.TIM_Prescaler=0;
	TIM_BaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_BaseStructure.TIM_Period=800;
	TIM_BaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_BaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2,&TIM_BaseStructure);
	//TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//定时器中断
	
	//tim3
    TIM_BaseStructure.TIM_Prescaler=35999;
	TIM_BaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_BaseStructure.TIM_Period=699;
	TIM_BaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_BaseStructure.TIM_RepetitionCounter=0;	
	TIM_TimeBaseInit(TIM3,&TIM_BaseStructure);
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//定时器中断		
	/*********tim2 pwm 模式占空比设置*************/
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           //配置为PWM模式1  
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;              //使能输出捕获寄存器  
    TIM_OCInitStructure.TIM_Pulse =430;                                       //设置跳变值，当计数器计数到这个值时，电平发生跳变  
    TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;                    //当定时器计数值小于CCR1时为高电平  
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);                                    //使能通道2      
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  
      
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
    TIM_OCInitStructure.TIM_Pulse =400;                                       //设置通道3的电平跳变值，输出另外一个占空比的PWM  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                    //当定时器计数值小于CCR2时为低电平 
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);                                    //使能通道3  
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); 	
    TIM_ARRPreloadConfig(TIM2, ENABLE);                                         //使能TIM2重载寄存器ARR
	TIM_CtrlPWMOutputs(TIM2,ENABLE);	
	TIM_Cmd(TIM2, ENABLE); 											//使能pwm输出
}	
////////////////////////////////////////
//小车行走函数
///////////////////////////////////////

void STOP(void)
{	
	IN1_H;
	IN2_H;
	IN3_H;
	IN4_H;		
	
}
void MOVE_ON(void)
{	
	IN1_L;
	IN2_H;
	IN3_L;
	IN4_H;			
}
void GO_BACK(void)
{	
	IN1_H;		//右边轮往后
	IN2_L;
	IN3_H;		//左边轮往后
	IN4_L;
}
void fast_right(void)
{
	IN1_L;		//右边轮往后
	IN2_H;
	IN3_H;		//左边轮往前
	IN4_L;	
}	
void TURN_RIGHT_90(void)
{
	IN1_L;		//右边轮往后
	IN2_H;
	IN3_H;		//左边轮往前
	IN4_L;		
}
void fast_left(void)
{
	IN1_H;		//右边轮往前
	IN2_L;		
	IN3_L;		//左边轮往后
	IN4_H;
}	
void TURN_LEFT_90(void)
{                
	IN1_H;		//右边轮往前
	IN2_L;		
	IN3_L;		//左边轮往后
	IN4_H;
}
void TURN_RIGHT(void)
{
	IN1_L;		//右边轮停止
	IN2_H;
	IN3_L;		//左边轮往前
	IN4_L;		
}
void TURN_LEFT(void)
{
	IN1_L;		//右边轮往前
	IN2_L;		
	IN3_L;		//左边轮停止
	IN4_H;
}

void TURN_LEFT_BACK(void)
{
	IN1_H;		//右边轮往前
	IN2_L;		
	IN3_L;		//左边轮停止
	IN4_L;
}

void SPEED(u8 LEVEL)//pwm速度设置
{	
	switch(LEVEL)
	{
		case 1:{TIM_SetCompare3(TIM2,375);TIM_SetCompare4(TIM2,375);}break;//220 代表存进CCR2寄存器的值，说明220就是定时器捕获的时间。
		case 2:{TIM_SetCompare3(TIM2,380);TIM_SetCompare4(TIM2,380);}break;  //3是右轮 4是左轮 右轮阻力大 
		case 3:{TIM_SetCompare3(TIM2,240);TIM_SetCompare4(TIM2,240);}break;
		case 4:{TIM_SetCompare3(TIM2,400);TIM_SetCompare4(TIM2,400);}break;
		case 5:{TIM_SetCompare3(TIM2,0);TIM_SetCompare4(TIM2,0);}break;
	}
}

