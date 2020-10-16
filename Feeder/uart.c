#include "uart.h"
#include "configuration.h"
#include <avr/io.h>

#define UROUND(x) ((2UL*(x)+1)/2)

#define UART_BAUDRATE(BAUD_RATE) ((uint16_t)UROUND(F_CLK_PER * 64UL / 16 / BAUD_RATE))
const int uart_baudrates[UART_MAX_BAUDRATE + 1] = {UART_BAUDRATE(1200),  UART_BAUDRATE(2400),  UART_BAUDRATE(4800),
                                                   UART_BAUDRATE(9600),  UART_BAUDRATE(19200), UART_BAUDRATE(38400),
                                                   UART_BAUDRATE(57600), UART_BAUDRATE(115200)};

char UARTRead() {
    while (!(USART0.STATUS & USART_RXCIF_bm))
        ;
	return USART0.RXDATAL;
}

void UARTSend(char c) {
    while (!(USART0.STATUS & USART_DREIF_bm))
        ;
    USART0.TXDATAL = c;
	USART0.STATUS = USART_TXCIF_bm;
}

void UARTWaitTx() {
	while (!(USART0.STATUS & USART_TXCIF_bm))
		;
}

void UARTRx() {
    VPORTA.OUT &= ~PIN1_bm;
}

void UARTTx() {
    VPORTA.OUT |= PIN1_bm;
}

void UARTSet(int speed) {
    configuration.baudrate = speed;
    USART0.BAUD = uart_baudrates[speed];
}

void UARTInit() {
    VPORTA.OUT &= ~PIN1_bm;
    VPORTA.DIR &= ~PIN7_bm;
    VPORTA.DIR |= PIN6_bm | PIN1_bm;

    UARTSet(configuration.baudrate);
	
	USART0.CTRLB |= USART_TXEN_bm | USART_RXEN_bm;
}
