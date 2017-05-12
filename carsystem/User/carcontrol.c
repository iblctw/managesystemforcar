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
	
	/*********tim2 pwm Ƶ������*************/
	
    TIM_BaseStructure.TIM_Prescaler=0;
	TIM_BaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_BaseStructure.TIM_Period=800;
	TIM_BaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_BaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2,&TIM_BaseStructure);
	//TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//��ʱ���ж�
	
	//tim3
    TIM_BaseStructure.TIM_Prescaler=35999;
	TIM_BaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_BaseStructure.TIM_Period=699;
	TIM_BaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_BaseStructure.TIM_RepetitionCounter=0;	
	TIM_TimeBaseInit(TIM3,&TIM_BaseStructure);
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//��ʱ���ж�		
	/*********tim2 pwm ģʽռ�ձ�����*************/
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           //����ΪPWMģʽ1  
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;              //ʹ���������Ĵ���  
    TIM_OCInitStructure.TIM_Pulse =430;                                       //��������ֵ�������������������ֵʱ����ƽ��������  
    TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;                    //����ʱ������ֵС��CCR1ʱΪ�ߵ�ƽ  
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);                                    //ʹ��ͨ��2      
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  
      
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
    TIM_OCInitStructure.TIM_Pulse =400;                                       //����ͨ��3�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                    //����ʱ������ֵС��CCR2ʱΪ�͵�ƽ 
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);                                    //ʹ��ͨ��3  
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); 	
    TIM_ARRPreloadConfig(TIM2, ENABLE);                                         //ʹ��TIM2���ؼĴ���ARR
	TIM_CtrlPWMOutputs(TIM2,ENABLE);	
	TIM_Cmd(TIM2, ENABLE); 											//ʹ��pwm���
}	
////////////////////////////////////////
//С�����ߺ���
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
	IN1_H;		//�ұ�������
	IN2_L;
	IN3_H;		//���������
	IN4_L;
}
void fast_right(void)
{
	IN1_L;		//�ұ�������
	IN2_H;
	IN3_H;		//�������ǰ
	IN4_L;	
}	
void TURN_RIGHT_90(void)
{
	IN1_L;		//�ұ�������
	IN2_H;
	IN3_H;		//�������ǰ
	IN4_L;		
}
void fast_left(void)
{
	IN1_H;		//�ұ�����ǰ
	IN2_L;		
	IN3_L;		//���������
	IN4_H;
}	
void TURN_LEFT_90(void)
{                
	IN1_H;		//�ұ�����ǰ
	IN2_L;		
	IN3_L;		//���������
	IN4_H;
}
void TURN_RIGHT(void)
{
	IN1_L;		//�ұ���ֹͣ
	IN2_H;
	IN3_L;		//�������ǰ
	IN4_L;		
}
void TURN_LEFT(void)
{
	IN1_L;		//�ұ�����ǰ
	IN2_L;		
	IN3_L;		//�����ֹͣ
	IN4_H;
}

void TURN_LEFT_BACK(void)
{
	IN1_H;		//�ұ�����ǰ
	IN2_L;		
	IN3_L;		//�����ֹͣ
	IN4_L;
}

void SPEED(u8 LEVEL)//pwm�ٶ�����
{	
	switch(LEVEL)
	{
		case 1:{TIM_SetCompare3(TIM2,375);TIM_SetCompare4(TIM2,375);}break;//220 ������CCR2�Ĵ�����ֵ��˵��220���Ƕ�ʱ�������ʱ�䡣
		case 2:{TIM_SetCompare3(TIM2,380);TIM_SetCompare4(TIM2,380);}break;  //3������ 4������ ���������� 
		case 3:{TIM_SetCompare3(TIM2,240);TIM_SetCompare4(TIM2,240);}break;
		case 4:{TIM_SetCompare3(TIM2,400);TIM_SetCompare4(TIM2,400);}break;
		case 5:{TIM_SetCompare3(TIM2,0);TIM_SetCompare4(TIM2,0);}break;
	}
}

