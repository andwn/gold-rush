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

static uint16_t hits_pgreat, hits_great, hits_good, hits_bad, hits_poor;

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
	hits_pgreat = hits_great = hits_good = hits_bad = hits_poor = 0;
	for(uint16_t i = MAX_NOTE; --i;) note[i] = (Note) {};
}

#define IN_RANGE(y, range) \
	(y >= JUDGEMENT_LINE - range && y <= JUDGEMENT_LINE + range)

void notes_update() {
	for(uint16_t i = 0; i < MAX_NOTE; i++) {
		if(!note[i].live) continue;
		note[i].y_pos += NOTE_SPEED;
		if(note[i].y_pos >= JUDGEMENT_LINE + RANGE_POOR) {
			// Missed
			hits_poor++;
			note[i].live = FALSE;
		} else if(joy_press(note[i].button) && note[i].y_pos >= JUDGEMENT_LINE - RANGE_POOR) {
			if(IN_RANGE(note[i].y_pos, RANGE_PGREAT)) {
				// Perfect
				hits_pgreat++;
				note[i].live = FALSE;
			} else if(IN_RANGE(note[i].y_pos, RANGE_GREAT)) {
				// Great
				hits_great++;
				note[i].live = FALSE;
			} else if(IN_RANGE(note[i].y_pos, RANGE_GOOD)) {
				// Good
				hits_good++;
				note[i].live = FALSE;
			} else if(IN_RANGE(note[i].y_pos, RANGE_BAD)) {
				// Bad
				hits_bad++;
				note[i].live = FALSE;
			} else if(IN_RANGE(note[i].y_pos, RANGE_POOR)) {
				// Poor
				hits_poor++;
				note[i].live = FALSE;
			}
		} else {
			note[i].sprite.y = 0x80 + (note[i].y_pos >> NOTE_SFT);
			if(note[i].y_pos <= JUDGEMENT_LINE) vdp_sprites_add(&note[i].sprite, 1);
		}
	}
}

void notes_draw_score() {
	char str[40];
	sprintf(str, "%04hu  %04hu  %04hu  %04hu  %04hu",
			hits_pgreat, hits_great, hits_good, hits_bad, hits_poor);
	vdp_puts(VDP_PLAN_A, str, 2, 26);
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
