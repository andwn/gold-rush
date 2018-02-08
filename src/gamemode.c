#include "common.h"
#include "gamemode.h"
#include "joy.h"
#include "stdlib.h"
#include "vdp.h"
#include "xgm.h"
#include "resources.h"

void splash() {
	vdp_vsync();
	vdp_load_tiles(TS_Money.tiles, 16, TS_Money.numTile);
	for(uint16_t y = 0; y < 7; y++) {
		for(uint16_t x = 0; x < 7; x++) {
			vdp_map_set_xy(VDP_PLAN_B, 16 + x + y * 7, 12 + x, 12 + y);
		}
	}
	vdp_set_colors(0, PAL_Money.data, 16);
	
	while(TRUE) {
		joy_update();
		
		vdp_vsync();
	}
}

void game() {

}

void result() {

}
