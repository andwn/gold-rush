#include "common.h"
#include "stdlib.h"
#include "vdp.h"

extern uint32_t v_err_reg[8+8];
extern uint32_t v_err_pc;
extern uint32_t v_err_addr;
extern uint16_t v_err_ext1;
extern uint16_t v_err_ext2;
extern uint16_t v_err_sr;
extern uint16_t v_err_type;

void _error() {
	char buf[40];
	
	// Reset VDP
	vdp_init();
	vdp_reset();
	
	// Error type
	uint16_t x = 2, y = 2;
	switch(v_err_type) {
		case 0:
		sprintf(buf, "%s", "Bus Error");
		vdp_set_color(0, 0x080);
		break;
		case 1:
		sprintf(buf, "%s", "Address Error");
		vdp_set_color(0, 0x800);
		break;
		case 2:
		sprintf(buf, "%s", "Bad Instruction");
		vdp_set_color(0, 0x008);
		break;
		case 3:
		sprintf(buf, "%s", "Divide by Zero");
		vdp_set_color(0, 0x0A6);
		break;
		default:
		sprintf(buf, "%s", "Debug Trace");
		vdp_set_color(0, 0x222);
	}
	vdp_puts(VDP_PLAN_A, buf, x, y);
			
	// Registers
	x = 2; y = 4;
	for(uint16_t i = 0; i < 8; i++) {
		sprintf(buf, "D%hu=%08X", i, v_err_reg[i]);
		vdp_puts(VDP_PLAN_A, buf, x, y+i);
	}
	for(uint16_t i = 0; i < 8; i++) {
		sprintf(buf, "A%hu=%08X", i, v_err_reg[i+8]);
		vdp_puts(VDP_PLAN_A, buf, x, y+i+8);
	}
	
	// Other values
	x = 15; 
	switch(v_err_type) {
		case 0:
		case 1: // Bus / Address Error
		y = 20;
		sprintf(buf, "FUNC=%04hX   INST=%04hX", v_err_ext1, v_err_ext2);
		vdp_puts(VDP_PLAN_A, buf, x, y++);
		sprintf(buf, "ADDR=%06X", v_err_addr);
		vdp_puts(VDP_PLAN_A, buf, x, y++);
		break;
		case 2: // Illegal
		y = 21;
		sprintf(buf, "OV=%04hX", v_err_ext1);
		vdp_puts(VDP_PLAN_A, buf, x, y++);
		break;
	}
	x = 2; y = 20;
	sprintf(buf, "PC=%06X", v_err_pc);
	vdp_puts(VDP_PLAN_A, buf, x, y++);
	sprintf(buf, "SR=  %04hX", v_err_sr);
	vdp_puts(VDP_PLAN_A, buf, x, y++);
	
	// Stack dump
	x = 25; y = 2;
	vdp_puts(VDP_PLAN_A, "Stack", x, y);
	y = 4;
	uint32_t *sp = (uint32_t*) v_err_reg[15];
	for(uint16_t i = 0; i < 16; i++) {
		if((uint32_t) sp >= 0xFFC800) break;
		x = 15;
		sprintf(buf, "SP+%02X=%08X", i << 3, *sp);
		vdp_puts(VDP_PLAN_A, buf, x, y);
		sp++;
		
		if((uint32_t) sp >= 0xFFC800) break;
		x = 30;
		sprintf(buf, "%08X", *sp);
		vdp_puts(VDP_PLAN_A, buf, x, y);
		sp++;
		
		y++;
	}
	
	// Message
	if(v_err_type < 4) {
		x = 2; y = 23;
		vdp_puts(VDP_PLAN_A, "This shouldn't happen. Report it!", x, y++);
		vdp_puts(VDP_PLAN_A, "andy@skychase.zone", x + 8, y++);
		vdp_puts(VDP_PLAN_A, "twitter.com/donutgrind", x + 8, y++);
	}
	
	// R.I.P
	while(TRUE);
}

void _die() {
	v_err_type = 4;
	_error();
}
