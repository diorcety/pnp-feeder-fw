#include "feeder.h"
#include "configuration.h"
#include "mcu.h"
#include "periph.h"
#include <stdlib.h>

/* FEEDER */
enum sFeederState state;

uint16_t remaining_length;
uint16_t current_length;
uint16_t last_action;
uint16_t ms_ticks;

void FeederAdvance(uint16_t length) {
    remaining_length = length;
}

void FeederEnable() {
    configuration.options &= ~FEEDER_OPTION_DISABLED_bm;
    state = sIDLE;
    ms_ticks = 0;
    remaining_length = 0;
    current_length = 0;
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

static void FeederGoTo(uint8_t nstate, uint16_t pos) {
    state = nstate;
    last_action = ms_ticks;
    ServoSet(pos);
}

void FeederUpdate() {
    switch (state) {
    case sDISABLED:
        break;
    case sIDLE:
        if (remaining_length == 0)
            break;
    case sORIGIN:
        if (abs(ms_ticks - last_action) > configuration.time_to_settle) {
            if (remaining_length >= FEEDER_MECHANICAL_ADVANCE_LENGTH) {
                current_length = FEEDER_MECHANICAL_ADVANCE_LENGTH;
                FeederGoTo(sFULL, configuration.full);
            } else if (remaining_length >= FEEDER_MECHANICAL_ADVANCE_LENGTH / 2) {
                current_length = FEEDER_MECHANICAL_ADVANCE_LENGTH / 2;
                FeederGoTo(sHALF, configuration.half);
            } else {
                state = sIDLE;
            }
        }
        break;
    case sHALF:
    case sFULL:
        if (abs(ms_ticks - last_action) > configuration.time_to_settle) {
            remaining_length -= current_length;
            FeederGoTo(sORIGIN, configuration.origin);
        }
        break;
    }
}
