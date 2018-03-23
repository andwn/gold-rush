#include "common.h"
#include "joy.h"
#include "stdlib.h"
#include "vdp.h"

#include "note.h"

#define NOTE_SFT		(4)						// Fixed point shift
#define NOTE_SPEED		(0x28)					// Falling speed
#define JUDGEMENT_LINE	(172 << NOTE_SFT)
#define PERFECT_RANGE	(6 << NOTE_SFT)
#define CLOSE_RANGE		(9 << NOTE_SFT)
#define POOR_RANGE		(12 << NOTE_SFT)

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
	perfect_hits = close_hits = poor_hits = misses = 0;
	for(uint16_t i = MAX_NOTE; --i;) note[i] = (Note) {};
}

void notes_update() {
	for(uint16_t i = 0; i < MAX_NOTE; i++) {
		if(!note[i].live) continue;
		note[i].y_pos += NOTE_SPEED;
		if(note[i].y_pos >= JUDGEMENT_LINE + POOR_RANGE) {
			// Miss
			misses++;
			note[i].live = FALSE;
		} else if(joy_press(note[i].button) && note[i].y_pos >= JUDGEMENT_LINE - POOR_RANGE) {
			if(note[i].y_pos >= JUDGEMENT_LINE - PERFECT_RANGE 
					&& note[i].y_pos <= JUDGEMENT_LINE + PERFECT_RANGE) {
				// Perfect hit
				perfect_hits++;
				note[i].live = FALSE;
			} else if(note[i].y_pos >= JUDGEMENT_LINE - CLOSE_RANGE 
					&& note[i].y_pos <= JUDGEMENT_LINE + CLOSE_RANGE) {
				// Close hit
				close_hits++;
				note[i].live = FALSE;
			} else if(note[i].y_pos >= JUDGEMENT_LINE - POOR_RANGE 
					&& note[i].y_pos <= JUDGEMENT_LINE + POOR_RANGE) {
				// Poor hit
				poor_hits++;
				note[i].live = FALSE;
			}
		} else {
			note[i].sprite.y = 0x80 + (note[i].y_pos >> NOTE_SFT);
			vdp_sprites_add(&note[i].sprite, 1);
		}
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
