#include "common.h"
#include "stdlib.h"
#include "vdp.h"

#include "note.h"

#define NOTE_CREATE_Y	16
#define NOTE_JUDGE_Y	182

#define MAX_NOTE 24
typedef struct {
	VDPSprite sprite;
	uint16_t y;
	uint8_t column, y_speed;
} Note;
Note note[MAX_NOTE];

void notes_init() {
	memset(note, sizeof(Note), MAX_NOTE);
}

void notes_update() {
	
}

void note_create(uint8_t column) {
	for(uint16_t i = 0; i < MAX_NOTE; i++) {
		if(note[i].column) continue;
		note[i].column = column;
	}
}
