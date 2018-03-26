#include "common.h"
#include "video.h"
#include "vdp.h"
#include "resources.h"

#define ANIM_LEN	0x10

typedef struct { const TileSet* ts; const Palette* pal; uint16_t time; } Frame;
static const Frame frames[ANIM_LEN];

#define BUF0_INDEX	0x100
#define BUF1_INDEX	0x200

static uint16_t c_frame;
static uint16_t c_timer;
static uint16_t c_ntsc_adjust;

enum { DRAW_IDLE, DRAW_TILES1, DRAW_TILES2, DRAW_TILES3, DRAW_TILES4, DRAW_MAPS };
static uint16_t draw_status;
static uint16_t draw_buffer;

void video_init() {
	c_frame = c_timer = c_ntsc_adjust = 0;
	draw_status = DRAW_IDLE;
	draw_buffer = BUF1_INDEX;
}

void video_update() {
	if(c_frame >= ANIM_LEN) return; // End of video
	if(pal_mode && ++c_ntsc_adjust >= 6) {
		c_timer--;
		c_ntsc_adjust = 0;
	}
	if(++c_timer >= frames[c_frame].time) {
		// Next frame in animation
		c_timer = 0;
		if(++c_frame >= ANIM_LEN) return; // End of video
		draw_buffer ^= 0x300;
		draw_status = DRAW_TILES1;
	}
	uint16_t ind = 0;
	switch(draw_status) {
		case DRAW_IDLE: break;
		case DRAW_TILES4: ind += 63; /* fallthrough */
		case DRAW_TILES3: ind += 63; /* fallthrough */
		case DRAW_TILES2: ind += 63; /* fallthrough */
		case DRAW_TILES1: 
		{
			vdp_load_tiles(&frames[c_frame].ts->tiles[ind<<3], draw_buffer + ind, 63);
			draw_status++;
		}
		break;
		case DRAW_MAPS:
		{
			ind = draw_buffer;
			if(draw_buffer == BUF0_INDEX) {
				ind |= 0x4000;
				vdp_set_colors(32, frames[c_frame].pal->data, 16);
			} else {
				ind |= 0x6000;
				vdp_set_colors(48, frames[c_frame].pal->data, 16);
			}
			for(uint16_t y = 0; y < 14; y++) {
				uint16_t map[18];
				for(uint16_t x = 0; x < 18; x++) map[x] = ind++;
				vdp_map_set_hline(VDP_PLAN_A, map, 20, 6+y, 18);
			}
			draw_status = DRAW_IDLE;
		}
		break;
	}
}

static const Frame frames[ANIM_LEN] = {
	{ NULL, NULL, 1 },
	{ &TS_Vid01, &PAL_Vid01, 60 },
	{ &TS_Vid03, &PAL_Vid03, 60 },
	{ &TS_Vid01, &PAL_Vid01, 60 },
	{ &TS_Vid03, &PAL_Vid03, 60 },
	{ &TS_Vid01, &PAL_Vid01, 60 },
	{ &TS_Vid03, &PAL_Vid03, 60 },
	{ &TS_Vid01, &PAL_Vid01, 60 },
	{ &TS_Vid03, &PAL_Vid03, 60 },
	{ &TS_Vid01, &PAL_Vid01, 60 },
	{ &TS_Vid03, &PAL_Vid03, 60 },
	{ &TS_Vid01, &PAL_Vid01, 60 },
	{ &TS_Vid03, &PAL_Vid03, 60 },
	{ &TS_Vid01, &PAL_Vid01, 60 },
	{ &TS_Vid03, &PAL_Vid03, 60 },
	{ &TS_Vid01, &PAL_Vid01, 60 },
};

static const uint16_t vidmap[2][14][18] = {{
	{ 0x2100,0x2101,0x2102,0x2103,0x2104,0x2105,0x2106,0x2107,0x2108,
		0x2109,0x210A,0x210B,0x210C,0x210D,0x210E,0x210F,0x2110,0x2111, },
	{ 0x2100,0x2101,0x2102,0x2103,0x2104,0x2105,0x2106,0x2107,0x2108,
		0x2109,0x210A,0x210B,0x210C,0x210D,0x210E,0x210F,0x2110,0x2111, },
},{
	
}};
