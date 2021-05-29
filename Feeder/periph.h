#ifndef PERIPH_H_
#define PERIPH_H_
#include <stdint.h>

#define MOTOR_bm PIN0_bm
#define FEEDBACK_bm PIN2_bm
#define FEEDBACK_CTRL PIN2CTRL
#define PWM_bm PIN3_bm
#define DE_bm PIN1_bm
#define TX_bm PIN6_bm
#define RX_bm PIN7_bm

uint8_t FeedbackGet();
void FeedbackUpdate();
void FeedbackInit();

void MotorStart();
void MotorStop();
void MotorInit();
void MotorEnable();
void MotorDisable();
uint8_t MotorEnabled();

void ServoStart();
void ServoStop();
void ServoSet(uint16_t pos);
uint16_t ServoGet();
void ServoInit();

void TimerStart();
void TimerStop();
void TimerInit();

#endif /* PERIPH_H_ */
