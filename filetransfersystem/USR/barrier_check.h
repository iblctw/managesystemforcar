#include "pbdata.h"
#define TRIG_H 	 GPIO_SetBits(GPIOC,GPIO_Pin_14)
#define TRIG_L 	 GPIO_ResetBits(GPIOC,GPIO_Pin_14)
//#define ECHO     (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7))
#define ECHO_1     (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15))
#define ECHO_2     (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
#define ECHO_3     (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
#define ECHO_4     (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2))
#define ECHO_5     (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3))
#define ECHO_6     (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4))
//#define ECHO_7     GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)
//#define ECHO_8     GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)
#define TIM2_ON  TIM_Cmd(TIM2,ENABLE)
#define TIM2_OFF TIM_Cmd(TIM2,DISABLE)
//void RCC_Configuration(void);
//void GPIO_Configuration(void);
void TIM_Configuration(void);
char BARRIER_Check(unsigned char num);
