#include "common.h"
#include "joy.h"
#include "note.h"
#include "vdp.h"
#include "resources.h"

static const struct {
	uint16_t button;
	int16_t x;
	uint16_t spr_index;
	uint16_t spr_count;
} GLOW_DATA[8] = {
	{ BTN_DIR,   0x80 + 24,  0, 3 },
	{ BTN_START, 0x80 + 48,  3, 2 },
	{ BTN_X,     0x80 + 62,  5, 2 },
	{ BTN_A,     0x80 + 76,  7, 2 },
	{ BTN_Y,     0x80 + 90,  9, 2 },
	{ BTN_B,     0x80 + 104, 11, 2 },
	{ BTN_Z,     0x80 + 118, 13, 2 },
	{ BTN_C,     0x80 + 132, 15, 2 },
};
static uint16_t glow_time[8];
static VDPSprite glow_spr[17][2];

static const struct {
	int16_t x;
	uint16_t spr_attr;
	uint16_t spr_w;
} WORD_DATA[7] = {
	{ 0x80 + 48, 0x2300, 10 },
	{ 0x80 + 48, 0x2328, 10 },
	{ 0x80 + 48, 0x2350, 10 },
	{ 0x80 + 48, 0x2378, 10 },
	{ 0x80 + 56, 0x23A0, 8 },
	{ 0x80 + 64, 0x23C0, 6 },
	{ 0x80 + 56, 0x23D8, 8 },
};
static uint16_t word_type;
static uint16_t word_pgreat_cycle;
static uint16_t word_time;
static VDPSprite word_spr[3];

void effects_init() {
	for(uint16_t i = 0; i < 8; i++) {
		glow_time[i] = 0;
		int16_t x = GLOW_DATA[i].x;
		uint16_t spr_index = GLOW_DATA[i].spr_index;
		uint16_t spr_count = GLOW_DATA[i].spr_count;
		for(uint16_t s = spr_index; s < spr_index + spr_count; s++) {
			glow_spr[s][0].x = x;
			glow_spr[s][1].x = x;
			glow_spr[s][0].y = 0x80 + 112;
			glow_spr[s][1].y = 0x80 + 144;
			glow_spr[s][0].size = SPRITE_SIZE(1, 4);
			glow_spr[s][1].size = SPRITE_SIZE(1, 4);
			x += 8;
		}
	}
	word_time = 0;
}

void effect_show_word(uint16_t type, uint16_t time) {
	word_type = type;
	word_time = time;
	uint16_t index = type;
	if(type != PGREAT) index += 2;
	word_spr[0].x = WORD_DATA[index].x;
	word_spr[1].x = WORD_DATA[index].x + 32;
	word_spr[2].x = WORD_DATA[index].x + 64;
	word_spr[0].y = 0x80 + 80;
	word_spr[1].y = 0x80 + 80;
	word_spr[2].y = 0x80 + 80;
	uint16_t spr_w = WORD_DATA[index].spr_w;
	word_spr[0].size = SPRITE_SIZE(4, 4);
	word_spr[1].size = SPRITE_SIZE((spr_w == 6) ? 2 : 4, 4);
	word_spr[2].size = SPRITE_SIZE(2, 4);
	word_spr[0].attr = WORD_DATA[index].spr_attr;
	word_spr[1].attr = WORD_DATA[index].spr_attr + 16;
	word_spr[2].attr = WORD_DATA[index].spr_attr + 32;
}

void effects_update() {
	for(uint16_t i = 0; i < 8; i++) {
		if(joy_down(GLOW_DATA[i].button)) {
			glow_time[i] = 8;
		}
		if(glow_time[i]) {
			glow_time[i]--;
			uint16_t spr_index = GLOW_DATA[i].spr_index;
			uint16_t spr_count = GLOW_DATA[i].spr_count;
			for(uint16_t s = spr_index; s < spr_index + spr_count; s++) {
				uint16_t offset = glow_time[i] << 3;
				glow_spr[s][0].attr = TILE_EXTRA2INDEX + 56 - offset;
				glow_spr[s][1].attr = TILE_EXTRA2INDEX + 56 - offset + 4;
			}
			vdp_sprites_add(glow_spr[spr_index], spr_count << 1);
		}
	}
	if(word_time) word_time--;
	if(word_time & 1) {
		if(word_type == PGREAT) {
			if(++word_pgreat_cycle > 2) word_pgreat_cycle = 0;
			word_spr[0].attr = WORD_DATA[word_pgreat_cycle].spr_attr;
			word_spr[1].attr = WORD_DATA[word_pgreat_cycle].spr_attr + 16;
			word_spr[2].attr = WORD_DATA[word_pgreat_cycle].spr_attr + 32;
		}
		vdp_sprites_add(word_spr, (word_type > GREAT) ? 2 : 3);
	}
}
