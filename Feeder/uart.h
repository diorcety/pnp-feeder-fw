#ifndef UART_H_
#define UART_H_
#include "mcu.h"

#define UARTCheckedRead()                                                                                              \
    c = UARTRead();                                                                                                    \
    if (c == C_RST)                                                                                                    \
        MCUReset();
		
#define UART_MAX_BAUDRATE 7

char UARTRead();
void UARTSend(char c);
void UARTRx();
void UARTTx();
void UARTWaitTx();
void UARTSet(int speed);
void UARTInit();

#endif /* UART_H_ */
