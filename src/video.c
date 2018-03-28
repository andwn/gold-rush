#include "common.h"
#include "video.h"
#include "vdp.h"
#include "resources.h"

#define NUM_IMAGES	44
#define ANIM_LEN	0x42

#define VID_WIDTH	18
#define VID_HEIGHT	14

#define BUF0_INDEX	0x100
#define BUF1_INDEX	0x200

typedef struct { const TileSet* ts; const Palette* pal; } Image;
static const Image images[NUM_IMAGES];
typedef struct { uint16_t time; uint16_t image; } Frame;
static const Frame frames[ANIM_LEN];
static const uint16_t vidmap[2][VID_HEIGHT][VID_WIDTH];

static uint16_t c_frame;
static uint16_t c_timer;
static uint16_t c_ntsc_adjust;

enum { DRAW_IDLE, DRAW_TILES1, DRAW_TILES2, DRAW_TILES3, DRAW_MAPS };
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
	if(!draw_status || !frames[c_frame].image) return;
	uint16_t ind = 0;
	const Image *img = &images[frames[c_frame].image];
	switch(draw_status) {
		// A third of the tiles are drawn per frame (88 tiles = 2816 bytes)
		//case DRAW_TILES4: ind += 63; /* fallthrough */
		case DRAW_TILES3: ind += 87; /* fallthrough */
		case DRAW_TILES2: ind += 88; /* fallthrough */
		case DRAW_TILES1: {
			vdp_load_tiles(&img->ts->tiles[ind<<3], draw_buffer + ind, 88);
			draw_status++;
		}
		break;
		// After drawing the tiles, apply the mapping to reveal the new image
		case DRAW_MAPS: {
			// ind becomes the array index for which mapping & palette
			if(draw_buffer == BUF1_INDEX) ind = 1;
			// Each frame has a unique palette
			vdp_set_colors(32 + (ind << 4), img->pal->data, 16);
			// Behind the image in PLAN_B is a square of color index 31
			// Stick the "transparent" color there so it becomes visible
			vdp_set_color(31, img->pal->data[0]);
			for(uint16_t y = 0; y < VID_HEIGHT; y++) {
				vdp_map_set_hline(VDP_PLAN_A, vidmap[ind][y], 20, 6+y, VID_WIDTH);
			}
			draw_status = DRAW_IDLE;
		}
		break;
	}
}

static const Image images[NUM_IMAGES] = {
	{ NULL, NULL },
	{ &TS_Vid01, &PAL_Vid01 },
	{ &TS_Vid02, &PAL_Vid02 },
	{ &TS_Vid03, &PAL_Vid03 },
	{ &TS_Vid04, &PAL_Vid04 },
	{ &TS_Vid05, &PAL_Vid05 },
	{ &TS_Vid06, &PAL_Vid06 },
	{ &TS_Vid07, &PAL_Vid07 },
	{ &TS_Vid08, &PAL_Vid08 },
	{ &TS_Vid09, &PAL_Vid09 },
	{ &TS_Vid10, &PAL_Vid10 },
	{ &TS_Vid11, &PAL_Vid11 },
	{ &TS_Vid12, &PAL_Vid12 },
	{ &TS_Vid13, &PAL_Vid13 },
	{ &TS_Vid14, &PAL_Vid14 },
	{ &TS_Vid15, &PAL_Vid15 },
	{ &TS_Vid16, &PAL_Vid16 },
	{ &TS_Vid17, &PAL_Vid17 },
	{ &TS_Vid18, &PAL_Vid18 },
	{ &TS_Vid19, &PAL_Vid19 },
	{ &TS_Vid20, &PAL_Vid20 },
	{ &TS_Vid21, &PAL_Vid21 },
	{ &TS_Vid22, &PAL_Vid22 },
	{ &TS_Vid23, &PAL_Vid23 },
	{ &TS_Vid24, &PAL_Vid24 },
	{ &TS_Vid25, &PAL_Vid25 },
	{ &TS_Vid26, &PAL_Vid26 },
	{ &TS_Vid27, &PAL_Vid27 },
	{ &TS_Vid28, &PAL_Vid28 },
	{ &TS_Vid29, &PAL_Vid29 },
	{ &TS_Vid30, &PAL_Vid30 },
	{ &TS_Vid31, &PAL_Vid31 },
	{ &TS_Vid32, &PAL_Vid32 },
	{ &TS_Vid33, &PAL_Vid33 },
	{ &TS_Vid34, &PAL_Vid34 },
	{ &TS_Vid35, &PAL_Vid35 },
	{ &TS_Vid36, &PAL_Vid36 },
	{ &TS_Vid37, &PAL_Vid37 },
	{ &TS_Vid38, &PAL_Vid38 },
	{ &TS_Vid39, &PAL_Vid39 },
	{ &TS_Vid40, &PAL_Vid40 },
	{ &TS_Vid41, &PAL_Vid41 },
	{ &TS_Vid42, &PAL_Vid42 },
	{ &TS_Vid43, &PAL_Vid43 },
};

static const Frame frames[ANIM_LEN] = {
	{1,1}, 
	// Logo
	{600,1},
	// Make money
	{30,4},{30,5},{30,6},
	// Scrolling GOLD
	{5,24},{5,25},{5,26},{5,27},{5,28},{5,29},{5,30},{5,31},{5,32},{5,33},
	{5,34},{5,35},{5,36},{5,37},{5,38},{5,39},{5,40},{5,41},{5,42},{5,43},
	// Make money
	{30,4},{30,5},{30,6},
	// Scrolling GOLD
	{5,24},{5,25},{5,26},{5,27},{5,28},{5,29},{5,30},{5,31},{5,32},{5,33},
	{5,34},{5,35},{5,36},{5,37},{5,38},{5,39},{5,40},{5,41},{5,42},{5,43},
	// Make money
	{30,4},{30,5},{30,6},
	// IIDX Logos
	{60,10},{60,11},{60,12},{60,13},{60,14},{60,15},{60,16},{60,17},
	{60,18},{60,19},{60,20},{60,21},{60,22},{60,23},{120,1},
};

static const uint16_t vidmap[2][VID_HEIGHT][VID_WIDTH] = {{
	{ 0x4100, 0x4101, 0x4102, 0x4103, 0x4104, 0x4105, 0x4106, 0x4107, 0x4108, 0x4109, 0x410a, 0x410b, 0x410c, 0x410d, 0x410e, 0x410f, 0x4110, 0x4111,  },
	{ 0x4112, 0x4113, 0x4114, 0x4115, 0x4116, 0x4117, 0x4118, 0x4119, 0x411a, 0x411b, 0x411c, 0x411d, 0x411e, 0x411f, 0x4120, 0x4121, 0x4122, 0x4123,  },
	{ 0x4124, 0x4125, 0x4126, 0x4127, 0x4128, 0x4129, 0x412a, 0x412b, 0x412c, 0x412d, 0x412e, 0x412f, 0x4130, 0x4131, 0x4132, 0x4133, 0x4134, 0x4135,  },
	{ 0x4136, 0x4137, 0x4138, 0x4139, 0x413a, 0x413b, 0x413c, 0x413d, 0x413e, 0x413f, 0x4140, 0x4141, 0x4142, 0x4143, 0x4144, 0x4145, 0x4146, 0x4147,  },
	{ 0x4148, 0x4149, 0x414a, 0x414b, 0x414c, 0x414d, 0x414e, 0x414f, 0x4150, 0x4151, 0x4152, 0x4153, 0x4154, 0x4155, 0x4156, 0x4157, 0x4158, 0x4159,  },
	{ 0x415a, 0x415b, 0x415c, 0x415d, 0x415e, 0x415f, 0x4160, 0x4161, 0x4162, 0x4163, 0x4164, 0x4165, 0x4166, 0x4167, 0x4168, 0x4169, 0x416a, 0x416b,  },
	{ 0x416c, 0x416d, 0x416e, 0x416f, 0x4170, 0x4171, 0x4172, 0x4173, 0x4174, 0x4175, 0x4176, 0x4177, 0x4178, 0x4179, 0x417a, 0x417b, 0x417c, 0x417d,  },
	{ 0x417e, 0x417f, 0x4180, 0x4181, 0x4182, 0x4183, 0x4184, 0x4185, 0x4186, 0x4187, 0x4188, 0x4189, 0x418a, 0x418b, 0x418c, 0x418d, 0x418e, 0x418f,  },
	{ 0x4190, 0x4191, 0x4192, 0x4193, 0x4194, 0x4195, 0x4196, 0x4197, 0x4198, 0x4199, 0x419a, 0x419b, 0x419c, 0x419d, 0x419e, 0x419f, 0x41a0, 0x41a1,  },
	{ 0x41a2, 0x41a3, 0x41a4, 0x41a5, 0x41a6, 0x41a7, 0x41a8, 0x41a9, 0x41aa, 0x41ab, 0x41ac, 0x41ad, 0x41ae, 0x41af, 0x41b0, 0x41b1, 0x41b2, 0x41b3,  },
	{ 0x41b4, 0x41b5, 0x41b6, 0x41b7, 0x41b8, 0x41b9, 0x41ba, 0x41bb, 0x41bc, 0x41bd, 0x41be, 0x41bf, 0x41c0, 0x41c1, 0x41c2, 0x41c3, 0x41c4, 0x41c5,  },
	{ 0x41c6, 0x41c7, 0x41c8, 0x41c9, 0x41ca, 0x41cb, 0x41cc, 0x41cd, 0x41ce, 0x41cf, 0x41d0, 0x41d1, 0x41d2, 0x41d3, 0x41d4, 0x41d5, 0x41d6, 0x41d7,  },
	{ 0x41d8, 0x41d9, 0x41da, 0x41db, 0x41dc, 0x41dd, 0x41de, 0x41df, 0x41e0, 0x41e1, 0x41e2, 0x41e3, 0x41e4, 0x41e5, 0x41e6, 0x41e7, 0x41e8, 0x41e9,  },
	{ 0x41ea, 0x41eb, 0x41ec, 0x41ed, 0x41ee, 0x41ef, 0x41f0, 0x41f1, 0x41f2, 0x41f3, 0x41f4, 0x41f5, 0x41f6, 0x41f7, 0x41f8, 0x41f9, 0x41fa, 0x41fb,  },
},{
	{ 0x6200, 0x6201, 0x6202, 0x6203, 0x6204, 0x6205, 0x6206, 0x6207, 0x6208, 0x6209, 0x620a, 0x620b, 0x620c, 0x620d, 0x620e, 0x620f, 0x6210, 0x6211,  },
	{ 0x6212, 0x6213, 0x6214, 0x6215, 0x6216, 0x6217, 0x6218, 0x6219, 0x621a, 0x621b, 0x621c, 0x621d, 0x621e, 0x621f, 0x6220, 0x6221, 0x6222, 0x6223,  },
	{ 0x6224, 0x6225, 0x6226, 0x6227, 0x6228, 0x6229, 0x622a, 0x622b, 0x622c, 0x622d, 0x622e, 0x622f, 0x6230, 0x6231, 0x6232, 0x6233, 0x6234, 0x6235,  },
	{ 0x6236, 0x6237, 0x6238, 0x6239, 0x623a, 0x623b, 0x623c, 0x623d, 0x623e, 0x623f, 0x6240, 0x6241, 0x6242, 0x6243, 0x6244, 0x6245, 0x6246, 0x6247,  },
	{ 0x6248, 0x6249, 0x624a, 0x624b, 0x624c, 0x624d, 0x624e, 0x624f, 0x6250, 0x6251, 0x6252, 0x6253, 0x6254, 0x6255, 0x6256, 0x6257, 0x6258, 0x6259,  },
	{ 0x625a, 0x625b, 0x625c, 0x625d, 0x625e, 0x625f, 0x6260, 0x6261, 0x6262, 0x6263, 0x6264, 0x6265, 0x6266, 0x6267, 0x6268, 0x6269, 0x626a, 0x626b,  },
	{ 0x626c, 0x626d, 0x626e, 0x626f, 0x6270, 0x6271, 0x6272, 0x6273, 0x6274, 0x6275, 0x6276, 0x6277, 0x6278, 0x6279, 0x627a, 0x627b, 0x627c, 0x627d,  },
	{ 0x627e, 0x627f, 0x6280, 0x6281, 0x6282, 0x6283, 0x6284, 0x6285, 0x6286, 0x6287, 0x6288, 0x6289, 0x628a, 0x628b, 0x628c, 0x628d, 0x628e, 0x628f,  },
	{ 0x6290, 0x6291, 0x6292, 0x6293, 0x6294, 0x6295, 0x6296, 0x6297, 0x6298, 0x6299, 0x629a, 0x629b, 0x629c, 0x629d, 0x629e, 0x629f, 0x62a0, 0x62a1,  },
	{ 0x62a2, 0x62a3, 0x62a4, 0x62a5, 0x62a6, 0x62a7, 0x62a8, 0x62a9, 0x62aa, 0x62ab, 0x62ac, 0x62ad, 0x62ae, 0x62af, 0x62b0, 0x62b1, 0x62b2, 0x62b3,  },
	{ 0x62b4, 0x62b5, 0x62b6, 0x62b7, 0x62b8, 0x62b9, 0x62ba, 0x62bb, 0x62bc, 0x62bd, 0x62be, 0x62bf, 0x62c0, 0x62c1, 0x62c2, 0x62c3, 0x62c4, 0x62c5,  },
	{ 0x62c6, 0x62c7, 0x62c8, 0x62c9, 0x62ca, 0x62cb, 0x62cc, 0x62cd, 0x62ce, 0x62cf, 0x62d0, 0x62d1, 0x62d2, 0x62d3, 0x62d4, 0x62d5, 0x62d6, 0x62d7,  },
	{ 0x62d8, 0x62d9, 0x62da, 0x62db, 0x62dc, 0x62dd, 0x62de, 0x62df, 0x62e0, 0x62e1, 0x62e2, 0x62e3, 0x62e4, 0x62e5, 0x62e6, 0x62e7, 0x62e8, 0x62e9,  },
	{ 0x62ea, 0x62eb, 0x62ec, 0x62ed, 0x62ee, 0x62ef, 0x62f0, 0x62f1, 0x62f2, 0x62f3, 0x62f4, 0x62f5, 0x62f6, 0x62f7, 0x62f8, 0x62f9, 0x62fa, 0x62fb,  },
}};
