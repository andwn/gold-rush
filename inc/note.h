enum { PGREAT, GREAT, GOOD, BAD, POOR };

void notes_set_delay(int16_t delay);
void notes_set_easy(uint16_t easy);
uint16_t notes_get_winner();

void notes_init();
void notes_update();
void notes_draw_score();

void note_create(uint8_t index);
