#ifndef _MOTOR_H
#define _MOTOR_H

void MOTOR0_Stop(void);
void MOTOR0_Foreward(unsigned int ms);
void MOTOR0_Reverse(unsigned int ms);

void MOTOR1_Stop(void);
void MOTOR1_Foreward(unsigned int ms);
void MOTOR1_Reverse(unsigned int ms);

void elevator_Stop(void);
void elevator_Foreward(unsigned int ms);
void elevator_Reverse(unsigned int ms);
#endif
