#include "common.h"
#include "stdlib.h"
#include "vdp.h"

#include "note.h"

#define NOTE_SPEED		(2)
#define NOTE_CREATE_Y	(0x80 + 16)
#define NOTE_EARLY_Y	(0x80 + 180)
#define NOTE_PERFECT_Y	(0x80 + 182)
#define NOTE_LATE_Y		(0x80 + 184)

enum { COLOR_RED, COLOR_WHITE, COLOR_BLUE };

static const struct {
	int16_t x;
	uint8_t color;
} NOTE_DATA[8] = { 
	{ 0x80 + 24,  COLOR_RED },
	{ 0x80 + 48,  COLOR_WHITE },
	{ 0x80 + 64,  COLOR_BLUE },
	{ 0x80 + 76,  COLOR_WHITE },
	{ 0x80 + 92,  COLOR_BLUE },
	{ 0x80 + 104, COLOR_WHITE },
	{ 0x80 + 120, COLOR_BLUE },
	{ 0x80 + 132, COLOR_WHITE },
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
} Note;
Note note[MAX_NOTE];

void notes_init() {
	for(uint16_t i = MAX_NOTE; --i;) note[i] = (Note) {};
}

void notes_update() {
	for(uint16_t i = 0; i < MAX_NOTE; i++) {
		if(!note[i].live) continue;
		note[i].sprite.y += NOTE_SPEED;
		if(note[i].sprite.y > NOTE_LATE_Y) {
			note[i].live = FALSE; // Delete note
		} else {
			vdp_sprites_add(&note[i].sprite, 1);
		}
	}
}

void note_create(uint8_t column) {
	for(uint16_t i = 0; i < MAX_NOTE; i++) {
		if(note[i].live) continue;
		uint8_t index = 0, color = 0;
		switch(column) {
			case NOTE_SCRATCH: index = 0; break;
			case NOTE_ST:      index = 1; break;
			case NOTE_X:       index = 2; break;
			case NOTE_A:       index = 3; break;
			case NOTE_Y:       index = 4; break;
			case NOTE_B:       index = 5; break;
			case NOTE_Z:       index = 6; break;
			case NOTE_C:       index = 7; break;
		}
		color = NOTE_DATA[index].color;
		note[i].sprite = (VDPSprite) {
			.x = NOTE_DATA[index].x,
			.y = NOTE_CREATE_Y,
			.size = COLOR_DATA[color].size,
			.attr = COLOR_DATA[color].attr,
		};
		note[i].live = TRUE;
		break;
	}
}
