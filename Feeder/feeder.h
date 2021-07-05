#ifndef FEEDER_H_
#define FEEDER_H_
#include <stdint.h>

#define FEEDER_FEEDBACK_DEBOUNCE 50
#define FEEDER_FEEDBACK_PRESS 500
#define FEEDER_MECHANICAL_ADVANCE_LENGTH 4
#define FEEDER_OPTION_DISABLED_bm 0x01
#define FEEDER_OPTION_IGNORE_FEEDBACK_bm 0x02
#define FEEDER_OPTION_MOTOR_DISABLED_bm 0x04
#define FEEDER_FEEDBACK_TRIGGERED(x) (!x)
#define FEEDER_MOTOR_ON 1
#define FEEDER_MOTOR_OFF 0
#define FEEDER_LENGTH_MIN (FEEDER_MECHANICAL_ADVANCE_LENGTH/2)
#define FEEDER_LENGTH_MAX (FEEDER_MECHANICAL_ADVANCE_LENGTH*8)
#define FEEDER_SERVO_MIN 500
#define FEEDER_SERVO_MAX 3000
#define FEEDER_TTS_MIN 0
#define FEEDER_TTS_MAX 10000

enum FeederState {
    sDISABLED = 0,
    sIDLE = 1,
    sMOVING = 2
};

enum FeederPosition {
    pORIGIN = 0,
    pHALF = 1,
    pFULL = 2
};

extern uint8_t state;
extern uint8_t position;

extern uint16_t remaining_length;
extern uint16_t last_action;
extern uint16_t ms_ticks;

void FeederAdvance(uint16_t length);
void FeederEnable();
void FeederDisable();
uint8_t FeederEnabled();
void FeedbackEnable();
void FeedbackDisable();
uint8_t FeedbackEnabled();
void FeederInit();
void FeederUpdate();

#endif /* FEEDER_H_ */
