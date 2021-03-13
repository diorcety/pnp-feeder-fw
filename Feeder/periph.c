#include "periph.h"
#include "configuration.h"
#include "feeder.h"
#include "mcu.h"
#include <avr/io.h>
#include <stdlib.h>

#define PWM_DIV (TCA_SINGLE_CLKSEL_DIV2_gc >> TCA_SINGLE_CLKSEL_gp)
#define SERVO_FREQUENCY 50
#define TCA_F(DIV) (F_CLK_PER / TCA_DIVIDER[DIV])
#define TCA_PERIOD(DIV, FREQ) (TCA_F(DIV) / FREQ - 1)
#define TCA_US(DIV, POS) ((POS * (TCA_F(DIV)/1000)) / (F_US/1000))
#define TCA_POS(DIV, US) ((US * (F_US/1000)) / (TCA_F(DIV)/1000))
	
static uint16_t feedback_ticks;
static uint8_t feedback_value;

uint8_t FeedbackGet() {
    return (VPORTA.IN >> PIN2_bp) & 0x1;
}

void FeedbackUpdate() {
    uint8_t feedback_new_value = FeedbackGet();
	
	/* Debouncing feedback */
	{
		static uint16_t feedback_ticks_raw = 0;
		static uint8_t feedback_value_raw;
		if (feedback_new_value != feedback_value_raw) {
			feedback_ticks_raw = ms_ticks;
			feedback_value_raw = feedback_new_value;
			return;
		} else {
			int16_t v = (int16_t)abs(ms_ticks - feedback_ticks_raw) - (int16_t)FEEDER_FEEDBACK_DEBOUNCE;
			if (v < 0) {
				return;
			} else {
				/* Avoid overflow issue */
				feedback_ticks_raw += v;
			}
		}
	}
	
	/*
	 * Debounced part
	 */

	int16_t feedback_action_ticks = (int16_t)abs(ms_ticks - feedback_ticks) - (int16_t)FEEDER_FEEDBACK_PRESS;
	
	/* Feedback update ? */
	if (feedback_new_value != feedback_value) {
		/* Idle state ? */
		if (state == sIDLE) {
			/* Short press occurs ? */
			if (feedback_action_ticks < 0) {
				if (FEEDER_FEEDBACK_TRIGGERED(feedback_value)) {
					if (FeedbackEnabled()) {
                        FeederAdvance(configuration.feed_length);
					}
				}
				/* Acknowledge action */
				feedback_ticks += feedback_action_ticks;
			} else {
				/* Previous state was a steady one -> start action ticker */
				feedback_ticks = ms_ticks;
			}
		} else {
			/* Ignore action */
			feedback_ticks += feedback_action_ticks;
		}
		
		/* Update motor state */
		(FEEDER_FEEDBACK_TRIGGERED(feedback_new_value) || !MotorEnabled()) ? MotorStop() : MotorStart();
			
		/* Update last feedback change value */
		feedback_value = feedback_new_value;
	} else {
		/* Avoid overflow issue */
		if (feedback_action_ticks >= 0) {
			feedback_ticks += feedback_action_ticks;
		}
	}
}

void FeedbackInit() {
    /* Feedback */
    VPORTA.DIR &= ~PIN2_bm;
    PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;

	feedback_ticks = 0;
	feedback_value = FeedbackGet();
}

void MotorStart() {
    VPORTA.OUT |= PIN0_bm;
}

void MotorStop() {
    VPORTA.OUT &= ~PIN0_bm;
}

void MotorInit() {
    /* Motor */
    VPORTA.OUT &= ~PIN0_bm;
    VPORTA.DIR |= PIN0_bm;
}

void ServoStart() {
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

void ServoStop() {
    TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
}

void ServoSet(uint16_t pos) {
	uint16_t v = TCA_US(PWM_DIV, pos);
    TCA0.SINGLE.CMP0 = v;
}

uint16_t ServoGet() {
	uint16_t v = TCA_POS(PWM_DIV, TCA0.SINGLE.CMP0);
	return v;
}

void ServoInit() {
    /* PWM */
    VPORTA.DIR |= (PIN3_bm);

    /* Set Normal mode */
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm                /* Update pin              */
                        | TCA_SINGLE_WGMODE_SINGLESLOPE_gc; /* Wave: SingleSlope(PWM)  */

    /* Set the period */
    TCA0.SINGLE.PER = TCA_PERIOD(PWM_DIV, SERVO_FREQUENCY);

    /* Start the PWM */
    TCA0.SINGLE.CTRLA = PWM_DIV << TCA_SINGLE_CLKSEL_gp;    /* Divider: 2              */

    ServoSet(configuration.origin);
}

void TimerStart() {
	while (RTC.STATUS > 0)
	; /* Wait for all register to be synchronized */
    RTC.PITCTRLA |= RTC_PITEN_bm;
}

void TimerStop() {
    RTC.PITCTRLA &= ~RTC_PITEN_bm;
}

void TimerInit() {
    RTC.CTRLA = RTC_PRESCALER_DIV1_gc   /* Pre-scaler: 1                    */
                | 0 << RTC_RTCEN_bp;    /* 1: enable RTC                    */
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;  /* clock source internal 32kHz RC   */
    RTC.PITCTRLA = RTC_PERIOD_CYC32_gc; /* RTC Clock Cycles 32768           */
	RTC.PITINTCTRL = RTC_PI_bm;         /* Periodic Interrupt: enabled      */
}
