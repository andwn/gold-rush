#include "common.h"
#include "effect.h"
#include "joy.h"
#include "stdlib.h"
#include "vdp.h"

#include "note.h"

#define NOTE_SFT		(4)						// Fixed point shift
#define NOTE_SPEED		(0x28)					// Falling speed
#define JUDGEMENT_LINE	(172 << NOTE_SFT)
#define BAD_EARLY		(JUDGEMENT_LINE - NOTE_SPEED * 10)
#define GOOD_EARLY		(JUDGEMENT_LINE - NOTE_SPEED * 8)
#define GREAT_EARLY		(JUDGEMENT_LINE - NOTE_SPEED * 3)
#define PGREAT_EARLY	(JUDGEMENT_LINE - NOTE_SPEED * 1)
#define PGREAT_LATE		(JUDGEMENT_LINE + NOTE_SPEED * 0)
#define GREAT_LATE		(JUDGEMENT_LINE + NOTE_SPEED * 1)
#define GOOD_LATE		(JUDGEMENT_LINE + NOTE_SPEED * 6)
#define BAD_LATE		(JUDGEMENT_LINE + NOTE_SPEED * 9)

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
	effect_show_word(type, 50);
}

void notes_update() {
	for(uint16_t i = 0; i < MAX_NOTE; i++) {
		if(!note[i].live) continue;
		note[i].y_pos += NOTE_SPEED;
		if(note[i].y_pos > BAD_LATE) {
			note_hit(&note[i], POOR); // Missed
		} else if(joy_press(note[i].button) && note[i].y_pos >= BAD_EARLY) {
			if     (note[i].y_pos < GOOD_EARLY)   note_hit(&note[i], BAD);
			else if(note[i].y_pos < GREAT_EARLY)  note_hit(&note[i], GOOD);
			else if(note[i].y_pos < PGREAT_EARLY) note_hit(&note[i], GREAT);
			else if(note[i].y_pos <= PGREAT_LATE) note_hit(&note[i], PGREAT);
			else if(note[i].y_pos <= GREAT_LATE)  note_hit(&note[i], GREAT);
			else if(note[i].y_pos <= GOOD_LATE)   note_hit(&note[i], GOOD);
			else if(note[i].y_pos <= BAD_LATE)    note_hit(&note[i], BAD);
		} else {
			note[i].sprite.y = 0x80 + (note[i].y_pos >> NOTE_SFT);
			if(note[i].y_pos < GREAT_LATE) {
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
		vdp_puts(VDP_PLAN_A, str, 19, 23);
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
