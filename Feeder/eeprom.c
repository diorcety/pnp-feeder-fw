#include "eeprom.h"
#include "configuration.h"
#include <avr/io.h>
#include <string.h>
#include <util/crc16.h>

#define CRC8_INITIAL_VALUE 0x0

static uint8_t EEPROMCrc() {
    uint8_t crc = CRC8_INITIAL_VALUE;
    uint8_t* ptr = (uint8_t*) &configuration;
    for (int i = 0; i < sizeof(struct Configuration) - 1; ++i) {
        crc = _crc8_ccitt_update(crc, ptr[i]);
    }
    return crc;
}

void EEPROMSave() {
    configuration.magic = EEPROMCrc();
    if (sizeof(struct Configuration) > EEPROM_PAGE_SIZE) {
        uint16_t eeprom_addr = EEPROM_START;
        uint8_t* configuration_addr = (uint8_t*) &configuration;
        uint8_t* configuration_addr_end = configuration_addr + sizeof(struct Configuration);
        while (configuration_addr < configuration_addr_end) {
            /* Clear page */
            _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);

            /* Copy */
            while (configuration_addr < configuration_addr_end) {
                *((uint8_t*) (eeprom_addr++)) = *(configuration_addr++);
                if (eeprom_addr % EEPROM_PAGE_SIZE == 0)
                    break;
            }

            /* Erase written part of page and program with desired value(s) */
            _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);

            /* Wait EEPROM flash end */
            while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm)
                ;
        }
    } else {
        /* Clear page */
        _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);

        /* Copy */
        memcpy((void*) EEPROM_START, &configuration, sizeof(struct Configuration));

        /* Erase written part of page and program with desired value(s) */
        _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);

        /* Wait EEPROM flash end */
        while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm)
            ;
    }
}

void EEPROMReset() {
    memcpy(&configuration, &defaultConfiguration, sizeof(struct Configuration));
    EEPROMSave();
}

void EEPROMLoad() {
    memcpy(&configuration, (void*) EEPROM_START, sizeof(struct Configuration));
    if (EEPROMCrc() != configuration.magic) {
        EEPROMReset();
    }
}

void EEPROMInit() {
    EEPROMLoad();
}
