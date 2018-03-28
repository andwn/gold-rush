#include "common.h"
#include "joy.h"
#include "stdlib.h"
#include "vdp.h"

#include "note.h"

#define NOTE_SFT		(4)						// Fixed point shift
#define NOTE_SPEED		(0x28)					// Falling speed
#define JUDGEMENT_LINE	(172 << NOTE_SFT)
#define RANGE_PGREAT	(3 << NOTE_SFT)
#define RANGE_GREAT		(6 << NOTE_SFT)
#define RANGE_GOOD		(9 << NOTE_SFT)
#define RANGE_BAD		(12 << NOTE_SFT)
#define RANGE_POOR		(15 << NOTE_SFT)

enum { PGREAT, GREAT, GOOD, BAD, POOR };

static uint16_t hits[5], old_hits[5];
static uint16_t bar_percent, bar_sub, old_percent;

enum { COLOR_RED, COLOR_WHITE, COLOR_BLUE };

static const struct {
	int16_t x;
	uint16_t color;
	uint16_t button;
} NOTE_DATA[8] = { 
	{ 0x80 + 24,  COLOR_RED,   BTN_DIR },
	{ 0x80 + 48,  COLOR_WHITE, BTN_START },
	{ 0x80 + 64,  COLOR_BLUE,  BTN_X },
	{ 0x80 + 76,  COLOR_WHITE, BTN_A },
	{ 0x80 + 92,  COLOR_BLUE,  BTN_Y },
	{ 0x80 + 104, COLOR_WHITE, BTN_B },
	{ 0x80 + 120, COLOR_BLUE,  BTN_Z },
	{ 0x80 + 132, COLOR_WHITE, BTN_C },
};

static const struct {
	uint16_t attr;
	uint8_t size;
} COLOR_DATA[3] = {
	{ TILE_NOTEINDEX + 4, SPRITE_SIZE(3,1), },
	{ TILE_NOTEINDEX + 2, SPRITE_SIZE(2,1), },
	{ TILE_NOTEINDEX,     SPRITE_SIZE(2,1), },
};

#define MAX_NOTE 24
typedef struct {
	VDPSprite sprite;
	uint16_t live;
	uint16_t y_pos;
	uint16_t button;
} Note;
Note note[MAX_NOTE];

void notes_init() {
	memset(hits, 0, sizeof(hits) << 1);
	memset(old_hits, 255, sizeof(hits) << 1);
	bar_percent = bar_sub = 0;
	old_percent = 255;
	for(uint16_t i = MAX_NOTE; --i;) note[i] = (Note) {};
}

static inline uint16_t in_range(int16_t y, int16_t range) {
	return y >= JUDGEMENT_LINE - range && y <= JUDGEMENT_LINE + range;
}

static void note_hit(Note *n, uint16_t type) {
	hits[type]++;
	n->live = FALSE;
	if(type != POOR) {
		if(++bar_sub >= 2) {
			bar_sub = 0;
			if(++bar_percent > 100) bar_percent = 100;
		}
	} else {
		if(bar_percent > 0) bar_percent--;
	}
}

void notes_update() {
	for(uint16_t i = 0; i < MAX_NOTE; i++) {
		if(!note[i].live) continue;
		note[i].y_pos += NOTE_SPEED;
		if(note[i].y_pos >= JUDGEMENT_LINE + RANGE_POOR) {
			note_hit(&note[i], POOR); // Missed
		} else if(joy_press(note[i].button) && note[i].y_pos >= JUDGEMENT_LINE - RANGE_POOR) {
			if(in_range(note[i].y_pos, RANGE_PGREAT)) {
				note_hit(&note[i], PGREAT);
			} else if(in_range(note[i].y_pos, RANGE_GREAT)) {
				note_hit(&note[i], GREAT);
			} else if(in_range(note[i].y_pos, RANGE_GOOD)) {
				note_hit(&note[i], GOOD);
			} else if(in_range(note[i].y_pos, RANGE_BAD)) {
				note_hit(&note[i], BAD);
			} else if(in_range(note[i].y_pos, RANGE_POOR)) {
				note_hit(&note[i], POOR);
			}
		} else {
			note[i].sprite.y = 0x80 + (note[i].y_pos >> NOTE_SFT);
			if(note[i].y_pos < JUDGEMENT_LINE + RANGE_PGREAT) {
				vdp_sprites_add(&note[i].sprite, 1);
			}
		}
	}
}

void notes_draw_score() {
	char str[40];
	uint16_t x = 2;
	for(uint16_t i = 0; i < 5; i++) {
		if(old_hits[i] != hits[i]) {
			sprintf(str, "%4hu", hits[i]);
			vdp_puts(VDP_PLAN_A, str, x, 26);
			old_hits[i] = hits[i];
		}
		x += 6;
	}
	if(old_percent != bar_percent) {
		sprintf(str, "%3hu", bar_percent);
		vdp_puts(VDP_PLAN_A, str, 17, 23);
		old_percent = bar_percent;
	}
}

void note_create(uint8_t type) {
	for(uint16_t i = 0; i < MAX_NOTE; i++) {
		if(!note[i].live) {
			uint8_t color = NOTE_DATA[type].color;
			note[i].sprite = (VDPSprite) {
				.x = NOTE_DATA[type].x, .y = 0,
				.size = COLOR_DATA[color].size,
				.attr = COLOR_DATA[color].attr,
			};
			note[i].live = TRUE;
			note[i].y_pos = 0;
			note[i].button = NOTE_DATA[type].button;
			break;
		}
	}
}
