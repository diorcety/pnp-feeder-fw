#ifndef PERIPH_H_
#define PERIPH_H_
#include <stdint.h>

uint8_t FeedbackGet();
void FeedbackUpdate();
void FeedbackInit();

void MotorStart();
void MotorStop();
void MotorInit();

void ServoStart();
void ServoStop();
void ServoSet(uint16_t pos);
uint16_t ServoGet();
void ServoInit();

void TimerStart();
void TimerStop();
void TimerInit();

#endif /* PERIPH_H_ */
