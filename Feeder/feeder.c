#include "feeder.h"
#include "configuration.h"
#include "mcu.h"
#include "periph.h"
#include <stdlib.h>

/* FEEDER */
enum FeederState state;
enum FeederPosition position;

uint16_t remaining_length;
uint16_t last_action;
uint16_t ms_ticks;

static void FeederGoTo(uint8_t nposition, uint16_t pos) {
    position = nposition;
    last_action = ms_ticks;
    ServoSet(pos);
}

void FeederAdvance(uint16_t length) {
    remaining_length = length;
    state = sMOVING;
}

void FeederEnable() {
    configuration.options &= ~FEEDER_OPTION_DISABLED_bm;
    state = sIDLE;
    ms_ticks = 0;
    remaining_length = 0;
    FeederGoTo(pORIGIN, configuration.origin);
    ServoStart();
    TimerStart();
}

void FeederDisable() {
    TimerStop();
    MotorStop();
    ServoStop();
    configuration.options |= FEEDER_OPTION_DISABLED_bm;
    state = sDISABLED;
}

uint8_t FeederEnabled() {
    return (configuration.options & FEEDER_OPTION_DISABLED_bm) == 0;
}

void FeedbackEnable() {
    configuration.options &= ~FEEDER_OPTION_IGNORE_FEEDBACK_bm;
}

void FeedbackDisable() {
    configuration.options |= FEEDER_OPTION_IGNORE_FEEDBACK_bm;
}

uint8_t FeedbackEnabled() {
    return (configuration.options & FEEDER_OPTION_IGNORE_FEEDBACK_bm) == 0;
}

void FeederInit() {
    if (FeederEnabled()) {
        FeederEnable();
    } else {
        FeederDisable();
    }
}

void FeederUpdate() {
    if (state != sMOVING) return;

    if (abs(ms_ticks - last_action) > configuration.time_to_settle) {
        switch(position)
        {
        case pORIGIN:
            if (remaining_length >= FEEDER_MECHANICAL_ADVANCE_LENGTH) {
                remaining_length -= FEEDER_MECHANICAL_ADVANCE_LENGTH;
                FeederGoTo(pFULL, configuration.full);
            } else if (remaining_length >= FEEDER_MECHANICAL_ADVANCE_LENGTH / 2) {
                remaining_length -= FEEDER_MECHANICAL_ADVANCE_LENGTH / 2;
                FeederGoTo(pHALF, configuration.half);
            } else {
                state = sIDLE;
            }
            break;
        case pHALF:
            if (remaining_length >= FEEDER_MECHANICAL_ADVANCE_LENGTH / 2) {
                remaining_length -= FEEDER_MECHANICAL_ADVANCE_LENGTH / 2;
                FeederGoTo(pFULL, configuration.full);
            } else {
                state = sIDLE;
            }
            break;
        case pFULL:
            FeederGoTo(pORIGIN, configuration.origin);
            break;
        }
    }
}
