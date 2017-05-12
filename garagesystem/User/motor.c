#include "pin_config.h"
#include "mfrc522.h"

//电机0
void MOTOR0_Stop(void)
{
	MOTOR0_IN1_L;
	MOTOR0_IN2_L;
	MOTOR0_IN3_L;
	MOTOR0_IN4_L;	
}

void MOTOR0_Foreward(unsigned int ms)
{
	MOTOR0_IN1_H;
	MOTOR0_IN2_H;
	MOTOR0_IN3_L;
	MOTOR0_IN4_L;
	delay_ms(ms);
		
	MOTOR0_IN2_H;
	MOTOR0_IN3_H;
	MOTOR0_IN1_L;
	MOTOR0_IN4_L;
	delay_ms(ms);
	
	MOTOR0_IN3_H;
	MOTOR0_IN4_H;
	MOTOR0_IN1_L;
	MOTOR0_IN2_L;
	delay_ms(ms);
	
	MOTOR0_IN1_H;
	MOTOR0_IN4_H;
	MOTOR0_IN2_L;
	MOTOR0_IN3_L;
	delay_ms(ms);
}

void MOTOR0_Reverse(unsigned int ms)
{
	MOTOR0_IN1_H;
	MOTOR0_IN4_H;
	MOTOR0_IN2_L;
	MOTOR0_IN3_L;
	delay_ms(ms);
		
	MOTOR0_IN3_H;
	MOTOR0_IN4_H;
	MOTOR0_IN1_L;
	MOTOR0_IN2_L;
	delay_ms(ms);
	
	MOTOR0_IN2_H;
	MOTOR0_IN3_H;
	MOTOR0_IN1_L;
	MOTOR0_IN4_L;
	delay_ms(ms);
	
	MOTOR0_IN1_H;
	MOTOR0_IN2_H;
	MOTOR0_IN3_L;
	MOTOR0_IN4_L;
	delay_ms(ms);
}
//电机1
void MOTOR1_Stop(void)
{
	MOTOR1_IN1_L;
	MOTOR1_IN2_L;
	MOTOR1_IN3_L;
	MOTOR1_IN4_L;	
}

void MOTOR1_Foreward(unsigned int ms)
{
	MOTOR1_IN1_H;
	MOTOR1_IN2_H;
	MOTOR1_IN3_L;
	MOTOR1_IN4_L;
	delay_ms(ms);
		
	MOTOR1_IN2_H;
	MOTOR1_IN3_H;
	MOTOR1_IN1_L;
	MOTOR1_IN4_L;
	delay_ms(ms);
	
	MOTOR1_IN3_H;
	MOTOR1_IN4_H;
	MOTOR1_IN1_L;
	MOTOR1_IN2_L;
	delay_ms(ms);
	
	MOTOR1_IN1_H;
	MOTOR1_IN4_H;
	MOTOR1_IN2_L;
	MOTOR1_IN3_L;
	delay_ms(ms);
}

void MOTOR1_Reverse(unsigned int ms)
{
	MOTOR1_IN1_H;
	MOTOR1_IN4_H;
	MOTOR1_IN2_L;
	MOTOR1_IN3_L;
	delay_ms(ms);
		
	MOTOR1_IN3_H;
	MOTOR1_IN4_H;
	MOTOR1_IN1_L;
	MOTOR1_IN2_L;
	delay_ms(ms);
	
	MOTOR1_IN2_H;
	MOTOR1_IN3_H;
	MOTOR1_IN1_L;
	MOTOR1_IN4_L;
	delay_ms(ms);
	
	MOTOR1_IN1_H;
	MOTOR1_IN2_H;
	MOTOR1_IN3_L;
	MOTOR1_IN4_L;
	delay_ms(ms);
}
//升降机
void elevator_Stop(void)
{
	elevator_IN1_L;
	elevator_IN2_L;
	elevator_IN3_L;
	elevator_IN4_L;	
}

void elevator_Foreward(unsigned int ms)
{
	elevator_IN1_H;
	elevator_IN2_H;
	elevator_IN3_L;
	elevator_IN4_L;
	delay_ms(ms);
		
	elevator_IN2_H;
	elevator_IN3_H;
	elevator_IN1_L;
	elevator_IN4_L;
	delay_ms(ms);
	
	elevator_IN3_H;
	elevator_IN4_H;
	elevator_IN1_L;
	elevator_IN2_L;
	delay_ms(ms);
	
	elevator_IN1_H;
	elevator_IN4_H;
	elevator_IN2_L;
	elevator_IN3_L;
	delay_ms(ms);
}

void elevator_Reverse(unsigned int ms)
{
	elevator_IN1_H;
	elevator_IN4_H;
	elevator_IN2_L;
	elevator_IN3_L;
	delay_ms(ms);
		
	elevator_IN3_H;
	elevator_IN4_H;
	elevator_IN1_L;
	elevator_IN2_L;
	delay_ms(ms);
	
	elevator_IN2_H;
	elevator_IN3_H;
	elevator_IN1_L;
	elevator_IN4_L;
	delay_ms(ms);
	
	elevator_IN1_H;
	elevator_IN2_H;
	elevator_IN3_L;
	elevator_IN4_L;
	delay_ms(ms);
}
