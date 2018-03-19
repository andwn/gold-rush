#include "common.h"
#include "xgm/pcm_drv.h"
#include "pcm.h"

static const uint8_t smp_null[0x100] __attribute__((aligned(256))) = {};

static volatile uint16_t* const z80_halt_port = (uint16_t*) 0xA11100;
static volatile uint16_t* const z80_reset_port = (uint16_t*) 0xA11200;

static uint8_t z80_bus_taken() {
    return (*z80_halt_port & 0x0100) ? FALSE : TRUE;
}

static void z80_request() {
    *z80_halt_port = 0x0100;
    *z80_reset_port = 0x0100;
    while(*z80_halt_port & 0x0100);
}

static void z80_release() {
    *z80_halt_port = 0x0000;
}

static void z80_reset_start() {
    *z80_reset_port = 0x0000;
}

static void z80_reset_end() {
    *z80_reset_port = 0x0100;
}

static void z80_load(const uint8_t *src, uint16_t size) {
    // copy data to Z80 RAM (need to use byte copy here)
    uint8_t* dst = (uint8_t*) 0xA00000;
    while(size--) *dst++ = *src++;
}

static volatile uint8_t* const z80_drv_command = (uint8_t*) 0xA00100;
static volatile uint8_t* const z80_drv_status =  (uint8_t*) 0xA00102;
static volatile uint8_t* const z80_drv_params =  (uint8_t*) 0xA00104;

static uint8_t pcm_ready() {
    // bus already taken ? just check status
    if(z80_bus_taken()) return *z80_drv_status & 0x80;
    // take the bus, check status and release bus
    z80_request();
    uint8_t rtn = *z80_drv_status & 0x80;
    z80_release();
    while(z80_bus_taken());
    return rtn;
}

void pcm_init() {
	// keep it real, load your drivers
    z80_request();
	z80_load(pcm_drv, sizeof(pcm_drv));
	// reset
	z80_reset_start();
	z80_release();
	while(z80_bus_taken());
	z80_reset_end();
	// point to Z80 null sample parameters
	z80_request();
	uint32_t addr = (uint32_t) smp_null;
	// null sample address (256 bytes aligned)
	z80_drv_params[0x20+0] = addr >> 8;
	z80_drv_params[0x20+1] = addr >> 16;
	// null sample length (256 bytes aligned)
	z80_drv_params[0x20+2] = sizeof(smp_null) >> 8;
	z80_drv_params[0x20+3] = sizeof(smp_null) >> 16;
	z80_release();
	while(z80_bus_taken());
    // wait for the driver to be ready
    while(!pcm_ready());
}

void pcm_play(const uint8_t *sample, const uint32_t len) {
	// give the sample address and command to play
	z80_request();
    uint32_t addr = (uint32_t) sample;
    // sample address (256 bytes aligned)
    z80_drv_params[0] = addr >> 8;
    z80_drv_params[1] = addr >> 16;
    // sample length (256 bytes aligned)
    z80_drv_params[2] = len >> 8;
    z80_drv_params[3] = len >> 16;
    z80_drv_params[4] = 1; // 22050hz rate
    z80_drv_params[6] = 0xC0; // pan center
    *z80_drv_command |= 1; // play command
    z80_drv_status[1] &= ~1; // no loop
    z80_release();
}

void pcm_stop() {
    z80_request();
    uint32_t addr = (uint32_t) smp_null;
    // sample address (256 bytes aligned)
    z80_drv_params[0x10+0] = addr >> 8;
    z80_drv_params[0x10+1] = addr >> 16;
    // sample length (256 bytes aligned)
    z80_drv_params[0x10+2] = sizeof(smp_null) >> 8;
    z80_drv_params[0x10+3] = sizeof(smp_null) >> 16;
    // remove play status
    z80_drv_status[0] &= ~1;
    z80_release();
}
