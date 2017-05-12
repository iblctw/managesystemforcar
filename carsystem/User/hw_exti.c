#include "hw_exti.h"

extern void delay_us(u32 nus);

void EXTI_peizhi(void)		//�ⲿ�ж�����
{
	EXTI_InitTypeDef EXTI_InitStructure;
									 
	EXTI_ClearITPendingBit(EXTI_Line11);								//����жϱ��λ
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);   	//��GPIO���ж��߿��ƺ�������ָ��GPIO��ĳ��Pin,
																//������ͽ������PA8����PB8��Ϊ�ж�Դ���������⣬��Ȼ��PB8��)
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;			   
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//�½��ش�����һ�����˸��½��ؾͻ����������
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	

}
u8 hw_js(void)	  //������5ms
{
	u8 t=0;
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==1)
	{
		t++;
		delay_us(20);
		if(t>=250) return t;
	}
	 return t;
				
}
