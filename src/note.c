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

static int16_t note_delay;
static uint16_t easy_mode;

static uint16_t hits[5], old_hits[5];
static uint16_t bar_percent, bar_sub, old_percent;

static const uint8_t div5[101] = {
	0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,
	5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8,9,9,9,9,9,
	10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,13,13,13,13,13,14,14,14,14,14,
	15,15,15,15,15,16,16,16,16,16,17,17,17,17,17,18,18,18,18,18,19,19,19,19,19,
	20,
};
static const uint8_t mod5[101] = {
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,
	0,
};

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
	int16_t y_pos;
	uint16_t button;
} Note;
Note note[MAX_NOTE];

void notes_set_delay(int16_t delay) {
	note_delay = delay * NOTE_SPEED;
}

void notes_set_easy(uint16_t easy) {
	easy_mode = easy;
}

uint16_t notes_get_winner() {
	return bar_percent | (easy_mode << 7);
}

void notes_init() {
	memset(hits, 0, sizeof(hits) << 1);
	memset(old_hits, 255, sizeof(hits) << 1);
	bar_percent = bar_sub = 0;
	old_percent = 1;
	for(uint16_t i = MAX_NOTE; i--;) note[i] = (Note) {};
}

static void note_hit(Note *n, uint16_t type) {
	hits[type]++;
	n->live = 2;
	switch(type) {
		case PGREAT: bar_sub += easy_mode ? 50 : 25; /* fallthrough */
		case GREAT: bar_sub += easy_mode ? 50 : 25; /* fallthrough */
		case GOOD: bar_sub += easy_mode ? 50 : 25;
		{
			while(bar_sub >= 100) {
				bar_sub -= 100;
				if(++bar_percent > 100) bar_percent = 100;
			}
		}
		break;
		case POOR: bar_percent -= easy_mode ? 1 : 2; /* fallthrough */
		case BAD: bar_percent -= easy_mode ? 1 : 2;
		{
			if(bar_percent > 100) bar_percent = 0;
		}
		break;
		
	}
	effect_show_word(type, (n->y_pos < PGREAT_EARLY - note_delay) ? 1 
						 : (n->y_pos >= GREAT_LATE - note_delay) ? 2 : 0);
}

void notes_update() {
	for(uint16_t i = 0; i < MAX_NOTE; i++) {
		if(!note[i].live) continue;
		note[i].y_pos += NOTE_SPEED;
		if(note[i].live == 1) {
			if(note[i].y_pos > BAD_LATE - note_delay) {
				note_hit(&note[i], joy_down(BTN_MODE) ? GREAT : POOR); // Missed
			} else if(joy_press(note[i].button) && note[i].y_pos >= BAD_EARLY - note_delay) {
				if     (note[i].y_pos < GOOD_EARLY - note_delay)   note_hit(&note[i], BAD);
				else if(note[i].y_pos < GREAT_EARLY - note_delay)  note_hit(&note[i], GOOD);
				else if(note[i].y_pos < PGREAT_EARLY - note_delay) note_hit(&note[i], GREAT);
				else if(note[i].y_pos <= PGREAT_LATE - note_delay) note_hit(&note[i], PGREAT);
				else if(note[i].y_pos <= GREAT_LATE - note_delay)  note_hit(&note[i], GREAT);
				else if(note[i].y_pos <= GOOD_LATE - note_delay)   note_hit(&note[i], GOOD);
				else if(note[i].y_pos <= BAD_LATE - note_delay)    note_hit(&note[i], BAD);
			}
		}
		note[i].sprite.y = 0x80 + ((note[i].y_pos - note_delay) >> NOTE_SFT);
		if(note[i].y_pos - note_delay < GREAT_LATE) {
			vdp_sprites_add(&note[i].sprite, 1);
		} else if(note[i].live == 2) {
			note[i].live = 0;
		}
	}
}

void notes_draw_score() {
	static const uint16_t x[5] = { 6, 12, 18, 24, 32 };
	char str[40];
	for(uint16_t i = 0; i < 5; i++) {
		if(old_hits[i] != hits[i]) {
			sprintf(str, "%3hu", hits[i]);
			vdp_puts(VDP_PLAN_A, str, x[i], 26);
			old_hits[i] = hits[i];
		}
	}
	if(old_percent != bar_percent) {
		sprintf(str, "%3hu", bar_percent);
		vdp_puts(VDP_PLAN_A, str, 19, 23);
		while(old_percent < bar_percent) {
			// Bar increasing
			if(mod5[++old_percent] == 0) {
				uint16_t fill = div5[old_percent];
				if(fill > 16) {
					vdp_map_set_xy(VDP_PLAN_A, TILE_BARINDEX+5, 3+fill-1, 24);
				} else {
					vdp_map_set_xy(VDP_PLAN_A, TILE_BARINDEX+2, 3+fill-1, 24);
				}
			}
		}
		while(old_percent > bar_percent) {
			// Bar decreasong
			if(mod5[--old_percent] == 0) {
				uint16_t fill = div5[old_percent];
				if(fill > 15) {
					vdp_map_set_xy(VDP_PLAN_A, TILE_BARINDEX+3, 3+fill, 24);
				} else {
					vdp_map_set_xy(VDP_PLAN_A, TILE_BARINDEX, 3+fill, 24);
				}
			}
		}
		//old_percent = bar_percent;
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
			note[i].y_pos = -(NOTE_SPEED * 20);
			note[i].button = NOTE_DATA[type].button;
			break;
		}
	}
}
