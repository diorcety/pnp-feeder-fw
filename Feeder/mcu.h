#ifndef MCU_H_
#define MCU_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

#define F_CLK_PER 2000000
#define F_US 1000000

extern const int TCA_DIVIDER[];

void MCUReset();
void MCUInit();

#endif /* MCU_H_ */
