#include "configuration.h"
#include "feeder.h"

const struct Configuration defaultConfiguration = {'Z',
                                                   3,
                                                   4,
                                                   750,
                                                   FEEDER_SERVO_MAX,
                                                   (FEEDER_SERVO_MAX + FEEDER_SERVO_MIN) / 2,
                                                   FEEDER_SERVO_MIN,
                                                   FEEDER_OPTION_DISABLED_bm | FEEDER_OPTION_MOTOR_DISABLED_bm,
                                                   0x0};
struct Configuration configuration;
