#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
#include <stdint.h>

struct Configuration {
    char name;
    uint8_t baudrate;
    uint8_t feed_length;
    uint16_t time_to_settle;
    uint16_t full;
    uint16_t half;
    uint16_t origin;
    uint8_t options;
    uint8_t magic;
};

extern const struct Configuration defaultConfiguration;
extern struct Configuration configuration;

#endif /* CONFIGURATION_H_ */
