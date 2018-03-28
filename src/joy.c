#include "common.h"
#include "joy.h"

void joy_init() {
	joystate = oldstate = 0;
    /* set the port bits direction */
    volatile uint8_t *pb = (volatile uint8_t*) 0xa10009;
    *pb = 0x40;
    pb += 2;
    *pb = 0x40;
    pb += 2;
    *pb = 0x40;
    /* set the port bits value */
    pb = (volatile uint8_t*) 0xa10003;
    *pb = 0x40;
    pb += 2;
    *pb = 0x40;
    pb += 2;
    *pb = 0x40;
}

void joy_update() {
	volatile uint8_t *pb = (volatile uint8_t*) 0xa10003;
	// On hardware, the C button did not work, but it did in emulators
    // Making this volatile fixed it for...reasons?
	volatile uint16_t v[4];
    for(uint16_t i = 0; i < 4; i++) {
		*pb = 0x00; /* TH (select) low */
		__asm__("nop");
		__asm__("nop");
		v[i] = *pb;
		*pb = 0x40; /* TH (select) high */
		v[i] <<= 8;
		v[i] |= *pb;
	}
	if((v[3] & 0x0F00) != 0x0F00) {
        v[2] = 0x0F00; // 3 button
    } else {
        v[2] = (v[2] & 0x000F) << 8; // 6 button
    }
	oldstate = joystate;
    joystate = ((v[0] & 0x3000) >> 6) | (v[0] & 0x003F);
    joystate |= v[2];
    joystate ^= 0x0FFF;
}
