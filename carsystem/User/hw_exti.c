#include "hw_exti.h"

extern void delay_us(u32 nus);

void EXTI_peizhi(void)		//外部中断设置
{
	EXTI_InitTypeDef EXTI_InitStructure;
									 
	EXTI_ClearITPendingBit(EXTI_Line11);								//清除中断标记位
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);   	//用GPIO的中断线控制函数启动指定GPIO的某个Pin,
																//（这里就解决了是PA8还是PB8作为中断源触发的问题，当然是PB8！)
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;			   
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//下降沿触发，一旦来了个下降沿就会进入服务程序
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	

}
u8 hw_js(void)	  //计数器5ms
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
