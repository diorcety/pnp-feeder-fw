#include "cmd.h"
#include "configuration.h"
#include "eeprom.h"
#include "feeder.h"
#include "periph.h"
#include "uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline int myatoi(const char* str, int* v) {
    int val = 0;
    while (*str) {
        char c = *str++;
        c -= '0';
        if (c < 0 || c > 9) {
            return 1;
        }
        val = val * 10 + c;
    }

    *v = val;
    return 0;
}

char cmd[MAX_CMD_SIZE + 1];

#define CMD_EQ(b) (cmd[0] == #b[0])
#define CMD_OK() goto OK;
#define CMD_ERROR_IC() goto ERROR_IC;
#define CMD_ERROR_IS() goto ERROR_IS;
#define CMD_ERROR_IV() goto ERROR_IV;
#define CMD_ERROR_IF() goto ERROR_IF;
#define CMD_REPLY_D(x)                                                                                                 \
    CmdReplyD(x);                                                                                                      \
    goto END;
#define CMD_REPLY_C(x)                                                                                                 \
    CmdReplyC(x);                                                                                                      \
    goto END;

#define arg (&cmd[2])
#define ARG_D_RANGE(min, max) (arg_is_int && arg_int >= min && arg_int <= max)
#define ARG_D_EQ(b) (arg_is_int && arg_int == b)
#define NO_ARG() (!with_arg)
#define IS_IDLE() (state == sIDLE && !FeedbackGet())

void CmdReplyD(int d) {
    itoa(d, arg, 10);
}

void CmdReplyC(char c) {
    arg[0] = c;
    arg[1] = '\0';
}

void Handle() {
    int with_arg = 0;
    int arg_is_int = 0;
    int arg_int = -1;

    /* Check command with/without argument */
    if (cmd[1] == '=') {
        with_arg = 1;
        arg_is_int = !myatoi(arg, &arg_int);
    } else if (cmd[1] != '\0') {
        CMD_ERROR_IC();
    }

    if (CMD_EQ(S)) {
        if (ARG_D_EQ(1)) {
            EEPROMSave();
            CMD_OK();
        } else {
            CMD_ERROR_IV();
        }
    } else if (CMD_EQ(R)) {
        if (ARG_D_EQ(1)) {
            EEPROMReset();
            CMD_OK();
        } else {
            CMD_ERROR_IV();
        }
    } else if (CMD_EQ(A)) {
        if (IS_IDLE()) {
            if (NO_ARG()) {
                FeederAdvance(configuration.feed_length);
                CMD_OK();
            } else {
                if (ARG_D_RANGE(FEEDER_LENGTH_MIN, FEEDER_LENGTH_MAX)) {
                    FeederAdvance(arg_int);
                    CMD_OK();
                } else {
                    CMD_ERROR_IV();
                }
            }
        } else {
            CMD_ERROR_IS();
        }
    } else if (CMD_EQ(V)) {
		if (NO_ARG()) {
			CMD_REPLY_D(state);
		}
    } else if (CMD_EQ(E)) {
        if (NO_ARG()) {
            CMD_REPLY_D(FeederEnabled());
        } else if (ARG_D_EQ(1)) {
            FeederEnable();
            CMD_OK();
        } else if (ARG_D_EQ(0)) {
            FeederDisable();
            CMD_OK();
        } else {
            CMD_ERROR_IV();
        }
    } else if (CMD_EQ(Z)) {
        if (NO_ARG()) {
            CMD_REPLY_D(FeedbackEnabled());
        } else if (ARG_D_EQ(1)) {
            FeedbackEnable();
            CMD_OK();
        } else if (ARG_D_EQ(0)) {
            FeedbackDisable();
            CMD_OK();
        } else {
            CMD_ERROR_IV();
        }
    } else if (CMD_EQ(N)) {
        if (NO_ARG()) {
            CMD_REPLY_C(configuration.name);
        } else {
            configuration.name = arg[0];
			CMD_OK();
        }
    } else if (CMD_EQ(B)) {
        if (NO_ARG()) {
            CMD_REPLY_D(configuration.baudrate);
        } else {
            if (ARG_D_RANGE(0, UART_MAX_BAUDRATE)) {
                UARTSet(arg_int);
				CMD_OK();
            } else {
                CMD_ERROR_IV();
            }
        }
    } else if (CMD_EQ(L)) {
        if (NO_ARG()) {
            CMD_REPLY_D(configuration.feed_length);
        } else {
            if (ARG_D_RANGE(FEEDER_LENGTH_MIN, FEEDER_LENGTH_MAX)) {
                configuration.feed_length = arg_int;
                CMD_OK();
            } else {
                CMD_ERROR_IV();
            }
        }
    } else if (CMD_EQ(T)) {
        if (NO_ARG()) {
            CMD_REPLY_D(configuration.time_to_settle);
        } else {
            if (ARG_D_RANGE(FEEDER_TTS_MIN, FEEDER_TTS_MAX)) {
                configuration.time_to_settle = arg_int;
                CMD_OK();
            } else {
                CMD_ERROR_IV();
            }
        }
    } else if (CMD_EQ(O)) {
        if (NO_ARG()) {
            CMD_REPLY_D(configuration.origin);
        } else {
            if (ARG_D_RANGE(FEEDER_SERVO_MIN, FEEDER_SERVO_MAX)) {
                configuration.origin = arg_int;
                CMD_OK();
            } else {
                CMD_ERROR_IV();
            }
        }
    } else if (CMD_EQ(H)) {
        if (NO_ARG()) {
            CMD_REPLY_D(configuration.half);
        } else {
            if (ARG_D_RANGE(FEEDER_SERVO_MIN, FEEDER_SERVO_MAX)) {
                configuration.half = arg_int;
                CMD_OK();
            } else {
                CMD_ERROR_IV();
            }
        }
    } else if (CMD_EQ(F)) {
        if (NO_ARG()) {
            CMD_REPLY_D(configuration.full);
        } else {
            if (ARG_D_RANGE(FEEDER_SERVO_MIN, FEEDER_SERVO_MAX)) {
                configuration.full = arg_int;
                CMD_OK();
            } else {
                CMD_ERROR_IV();
            }
        }
    } else if (CMD_EQ(P)) {
		if (NO_ARG()) {
			CMD_REPLY_D(ServoGet());
		} else {
			if (ARG_D_RANGE(FEEDER_SERVO_MIN, FEEDER_SERVO_MAX)) {
				ServoSet(arg_int);
				CMD_OK();
			} else {
				CMD_ERROR_IV();
			}
		}
    }

ERROR_IC:
    strcpy(arg, "XC");
    goto END;
ERROR_IS:
    strcpy(arg, "XS");
    goto END;
ERROR_IV:
    strcpy(arg, "XV");
    goto END;
OK:
    strcpy(arg, "OK");
    goto END;
END:
    cmd[1] = ':';
}
