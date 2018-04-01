#include "common.h"
#include "beatmap.h"
#include "effect.h"
#include "joy.h"
#include "note.h"
#include "pcm.h"
#include "stdlib.h"
#include "vdp.h"
#include "video.h"
#include "tiles_res.h"
#include "resources.h"

#include "gamemode.h"

void splash() {
	vdp_vsync();
	vdp_map_clear(VDP_PLAN_A);
	vdp_map_clear(VDP_PLAN_B);
	vdp_vsync();
	vdp_load_tiles(PAT_Money, 16, sizeof(PAT_Money) / 32);
	uint16_t index = 16;
	for(uint16_t y = 0; y < 7; y++) {
		for(uint16_t x = 0; x < 7; x++) {
			vdp_map_set_xy(VDP_PLAN_B, 0x2000 + index, 16 + x, 10 + y);
			index++;
		}
	}
	vdp_puts(VDP_PLAN_B, "< DELAY : +00.0 >", 11, 20);
	vdp_set_color(0, PAL_Money.data[0]);
	vdp_set_colors(16, PAL_Money.data, 16);
	
	char str[8];
	int16_t delay = 0;
	uint16_t easy_mode = 0;
	uint16_t cursor_time = 1;
	while(TRUE) {
		joy_update();
		if(joy_press(BTN_START | BTN_C)) {
			break;
		} else if(joy_press(BTN_A)) {
			easy_mode ^= 1;
		}
		if(joy_down(BTN_LEFT)) {
			if(delay > -50 && --cursor_time == 0) {
				cursor_time = 4;
				delay--;
			}
		} else if(joy_down(BTN_RIGHT)) { 
			if(delay < 50 && --cursor_time == 0) {
				cursor_time = 4;
				delay++;
			}
		} else {
			cursor_time = 1;
		}
		vdp_vsync();
		sprintf(str, "+%02hd.%01hd", abs(delay / 10), abs(delay % 10));
		vdp_puts(VDP_PLAN_B, str, 21, 20);
		if(delay < 0) vdp_puts(VDP_PLAN_B, "-", 21, 20);
		vdp_puts(VDP_PLAN_B, easy_mode ? "(A)  EASY " : "(A) NORMAL", 15, 22);
	}
	//if(delay < 0) delay -= 10;
	//beatmap_set_delay(delay / 10, delay % 10);
	notes_set_delay(delay);
	notes_set_easy(easy_mode);
	gamemode = GM_GAME;
}

void game() {
	vdp_vsync();
	// Background / layout
	vdp_load_tiles(PAT_Layout, 0, sizeof(PAT_Layout) / 32);
	uint16_t mapind = 40;
	for(uint16_t y = 0; y < 28; y++) {
		vdp_map_set_hline(VDP_PLAN_B, (uint16_t*) MAP_Layout + mapind, 0, y, 40);
		mapind += 40;
	}
	// Other tiles
	vdp_load_tiles(PAT_Notes, TILE_NOTEINDEX, sizeof(PAT_Notes) / 32);
	vdp_load_tiles(PAT_Bar, TILE_BARINDEX, sizeof(PAT_Bar) / 32);
	vdp_load_tiles(PAT_Glow, TILE_GLOWINDEX, sizeof(PAT_Glow) / 32);
	vdp_load_tiles(PAT_Words, TILE_WORDSINDEX, sizeof(PAT_Words) / 32);
	vdp_load_tiles(PAT_Speed, TILE_SPEEDINDEX, sizeof(PAT_Speed) / 32);
	// Some text that doesn't change later
	vdp_puts(VDP_PLAN_A, "PGreat Great  Good   Bad    Poor", 3, 25);
	vdp_puts(VDP_PLAN_A, "GOLD RUSH", 24, 2);
	vdp_puts(VDP_PLAN_A, "DJ Yoshitaka", 22, 3);
	vdp_puts(VDP_PLAN_A, "Michael a la mode", 20, 4);
	// Layout/note and video palettes
	vdp_set_colors(0, PAL_Layout.data, 16);
	vdp_set_colors(16, PAL_Words.data, 16);
	
	video_init();
	beatmap_init();
	notes_init();
	effects_init();
	
	while(TRUE) {
		joy_update();
		
		if(beatmap_update()) break;
		notes_update();
		effects_update();
		
		vdp_vsync();
		vdp_sprites_update();
		video_update();
		notes_draw_score();
		//beatmap_draw_debug();
	}
	gamemode = GM_RESULT;
}

void result() {
	vdp_vsync();
	vdp_sprites_clear();
	vdp_sprites_update();
	vdp_map_clear(VDP_PLAN_A);
	vdp_map_clear(VDP_PLAN_B);
	
	vdp_vsync();
	uint16_t x, y, w, h;
	uint16_t win = notes_get_winner();
	if(win & 128) { // EASY
		if((win & 127) >= 80) { // WINNER
			x = 1; y = 1; w = 38; h = 24;
			vdp_load_tiles(PAT_EasyWin, 2, w * h);
			vdp_set_colors(32, PAL_EasyWin.data, 16);
			vdp_puts(VDP_PLAN_A, "Congratulations!", 12, 26);
		} else { // LOSER
			x = 1; y = 0; w = 24; h = 27;
			vdp_load_tiles(PAT_EasyLose, 2, w * h);
			vdp_set_colors(32, PAL_EasyLose.data, 16);
			vdp_puts(VDP_PLAN_A, "Try Again", 24, 26);
		}
	} else { // NORMAL
		if((win & 127) >= 80) { // WINNER
			x = 1; y = 1; w = 38; h = 22;
			vdp_load_tiles(PAT_NormalWin, 2, w * h);
			vdp_set_colors(32, PAL_NormalWin.data, 16);
			vdp_puts(VDP_PLAN_A, "Congratulations!", 12, 26);
		} else { // LOSER
			x = 1; y = 1; w = 38; h = 21;
			vdp_load_tiles(PAT_NormalLose, 2, w * h);
			vdp_set_colors(32, PAL_NormalLose.data, 16);
			vdp_puts(VDP_PLAN_A, "Try Again", 15, 26);
		}
	}
	
	vdp_vsync();
	uint16_t index = 2;
	for(uint16_t yy = 0; yy < h; yy++) {
		for(uint16_t xx = 0; xx < w; xx++) {
			vdp_map_set_xy(VDP_PLAN_B, 0x4000 | index, x+xx, y+yy);
			index++;
		}
	}
	
	uint16_t timer = 50;
	while(TRUE) {
		joy_update();
		if(timer > 0) timer--;
		else if(joy_press(BTN_START | BTN_C)) {
			break;
		}
		vdp_vsync();
	}
	gamemode = GM_SPLASH;
}
