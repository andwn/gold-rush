#define NOTE_SCRATCH	0b10000000
#define NOTE_ST			0b01000000
#define NOTE_X			0b00100000
#define NOTE_A			0b00010000
#define NOTE_Y			0b00001000
#define NOTE_B			0b00000100
#define NOTE_Z			0b00000010
#define NOTE_C			0b00000001

void notes_init();
void notes_update();

void note_create(uint8_t index);
