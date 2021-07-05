#include "cmd.h"
#include "configuration.h"
#include "eeprom.h"
#include "feeder.h"
#include "mcu.h"
#include "periph.h"
#include "uart.h"

#define UARTCheckedRead() \
    c = UARTRead();       \
    if (c < 0)            \
        goto idle;        \
    if (c == C_RST)       \
        MCUReset();

int main(void) {
    MCUInit();
    EEPROMInit();
    UARTInit();
    FeedbackInit();
    ServoInit();
    MotorInit();
    TimerInit();
    FeederInit();
    
    TimerStart();

    int c;
    int i;

idle:
    UARTRx();

    /* Wait for frame start */
    do {
        UARTCheckedRead();
    } while (c != C_COM);

    /* Check the name */
    UARTCheckedRead();
    if (c != configuration.name && c != '*')
        goto discard;

    /* Get char until frame end */
    i = 0;
    while (1) {
        UARTCheckedRead();
        if (c == C_COM) {
            cmd[i] = '\0';
            break;
        } else {
            cmd[i++] = c;
            if (i > MAX_CMD_SIZE)
                i = MAX_CMD_SIZE;
        }
    }

    /* Handle the command */
    Handle();
    
    /* Prepare to reply */
    UARTTx();
    
    /* Wait a bit before replying */
    TimerDelay(3);

    /* Reply */
    i = 0;
    while (cmd[i] != '\0')
        UARTSend(cmd[i++]);
    UARTSend('\r');

    UARTWaitTx();

    goto idle;
discard:
    /* Wait for frame end */
    do {
        UARTCheckedRead();
    } while (c != C_COM);
    goto idle;
}

ISR(RTC_PIT_vect) {
    /* Clear flag by writing '1': */
    RTC.PITINTFLAGS = RTC_PI_bm;

    ms_ticks++;

    if (FeederEnabled()) {
        FeedbackUpdate();
        FeederUpdate();
    }
}
