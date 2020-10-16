#include "mcu.h"

const int TCA_DIVIDER[] = {1, 2, 4, 8, 16, 64, 256, 1024};

void MCUReset() {
    /* System reset */
    _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRE_bm);
}

void MCUInit() {
    /* 20Mhz */
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, 0x00);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0x09 << CLKCTRL_PDIV_gp | CLKCTRL_PEN_bm); // F_CLK_PER = 3333333
	
	sei();
}
