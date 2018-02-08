#include "common.h"
#include "stdlib.h"
#include "vdp.h"

#include "joy.h"

#define JOY_TYPE_SHIFT          12

static uint8_t joyType[2];
static uint16_t joyState[2];

void joy_init() {
	/*
     * Initialize ports for peripheral interface protocol - default to
     * TH Control Method for pads
     */
	volatile uint8_t *pb;
    /* set the port bits direction */
    pb = (volatile uint8_t *)0xa10009;
    *pb = 0x40;
    pb += 2;
    *pb = 0x40;
    pb += 2;
    *pb = 0x40;
    /* set the port bits value */
    pb = (volatile uint8_t *)0xa10003;
    *pb = 0x40;
    pb += 2;
    *pb = 0x40;
    pb += 2;
    *pb = 0x40;

    vdp_vsync();
    joy_update();
    vdp_vsync();
    joy_update();

    for(uint8_t i = 0; i < 2; i++) {
        joyType[i] = JOY_TYPE_PAD6;
        joyState[i] = 0;
    }
    /* wait a few vblanks for joy_update() to get valid data */
    vdp_vsync();
    joy_update();
    vdp_vsync();
    joy_update();
    vdp_vsync();
    joy_update();
}

uint8_t joy_type(uint16_t joy) {
    return joyType[joy];
}

uint16_t joy_read(uint16_t joy) {
    return joyState[joy];
}

static uint16_t TH_CONTROL_PHASE(volatile uint8_t *pb) {
    uint16_t val;

    *pb = 0x00; /* TH (select) low */
    __asm__("nop");
    __asm__("nop");
    val = *pb;

    *pb = 0x40; /* TH (select) high */
    val <<= 8;
    val |= *pb;

    return val;
}

static uint16_t read3Btn(uint16_t port) {
    volatile uint8_t *pb;
    uint16_t val;

    pb = (volatile uint8_t *)0xa10003 + port*2;

    val = TH_CONTROL_PHASE(pb);                   /* - 0 s a 0 0 d u - 1 c b r l d u */
    val = ((val & 0x3000) >> 6) | (val & 0x003F); /* 0 0 0 0 0 0 0 0 s a c b r l d u */
    val ^= 0x00FF;                                /* 0 0 0 0 0 0 0 0 S A C B R L D U */

    return val | (JOY_TYPE_PAD3 << JOY_TYPE_SHIFT);
}

static uint16_t read6Btn(uint16_t port) {
    volatile uint8_t *pb;
    uint16_t val, v1, v2;

    pb = (volatile uint8_t *)0xa10003 + port*2;

    v1 = TH_CONTROL_PHASE(pb);                    /* - 0 s a 0 0 d u - 1 c b r l d u */
    val = TH_CONTROL_PHASE(pb);                   /* - 0 s a 0 0 d u - 1 c b r l d u */
    v2 = TH_CONTROL_PHASE(pb);                    /* - 0 s a 0 0 0 0 - 1 c b m x y z */
    val = TH_CONTROL_PHASE(pb);                   /* - 0 s a 1 1 1 1 - 1 c b r l d u */

    if ((val & 0x0F00) != 0x0F00) v2 = (JOY_TYPE_PAD3 << JOY_TYPE_SHIFT) | 0x0F00; /* three button pad */
    else v2 = (JOY_TYPE_PAD6 << JOY_TYPE_SHIFT) | ((v2 & 0x000F) << 8); /* six button pad */

    val = ((v1 & 0x3000) >> 6) | (v1 & 0x003F);   /* 0 0 0 0 0 0 0 0 s a c b r l d u  */
    val |= v2;                                    /* 0 0 0 1 m x y z s a c b r l d u  or  0 0 0 0 1 1 1 1 s a c b r l d u */
    val ^= 0x0FFF;                                /* 0 0 0 1 M X Y Z S A C B R L D U  or  0 0 0 0 0 0 0 0 S A C B R L D U */

    return val;
}

void joy_update() {
    uint16_t val;
    uint16_t newstate;
	// Joy 1
	val = (joyType[0] == JOY_TYPE_PAD6) ? read6Btn(0) : read3Btn(0);
	newstate = val & BUTTON_ALL;
	joyType[0] = val >> JOY_TYPE_SHIFT;
	joyState[0] = newstate;
	// Joy 2
	val = (joyType[1] == JOY_TYPE_PAD6) ? read6Btn(1) : read3Btn(1);
	newstate = val & BUTTON_ALL;
	joyType[1] = val >> JOY_TYPE_SHIFT;
	joyState[1] = newstate;
}
