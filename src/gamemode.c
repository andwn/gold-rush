#include "common.h"
#include "gamemode.h"
#include "joy.h"
#include "stdlib.h"
#include "vdp.h"
#include "xgm.h"
#include "resources.h"

void splash() {
	vdp_vsync();
	vdp_map_clear(VDP_PLAN_A);
	vdp_map_clear(VDP_PLAN_B);
	vdp_vsync();
	vdp_load_tiles(TS_Money.tiles, 16, TS_Money.numTile);
	for(uint16_t y = 0; y < 7; y++) {
		for(uint16_t x = 0; x < 7; x++) {
			vdp_map_set_xy(VDP_PLAN_B, 16 + x + y * 7, 16 + x, 10 + y);
		}
	}
	vdp_set_colors(0, PAL_Money.data, 16);
	
	uint16_t splash_time = 180;
	while(TRUE) {
		joy_update();
		if(!--splash_time) break;
		vdp_vsync();
	}
	gamemode = GM_GAME;
}

void game() {
	//vdp_vsync();
	//vdp_map_clear(VDP_PLAN_A);
	//vdp_map_clear(VDP_PLAN_B);
	vdp_vsync();
	vdp_load_tiles(PAT_Layout, 0, sizeof(PAT_Layout) / 32);
	uint16_t mapind = 0;
	for(uint16_t y = 0; y < 28; y++) {
		vdp_map_set_hline(VDP_PLAN_B, (uint16_t*) MAP_Layout + mapind, 0, y, 40);
		mapind += 40;
	}
	vdp_set_colors(0, PAL_Layout.data, 16);
	
	while(TRUE) {
		joy_update();
		
		vdp_vsync();
	}
}

void result() {
	
}
