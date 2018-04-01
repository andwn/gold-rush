#define BEATMAP_LEN 0x400

//void beatmap_set_delay(int16_t frame_delay, int16_t sub_delay);
void beatmap_init();
uint8_t beatmap_update();
void beatmap_draw_score();
void beatmap_draw_debug();
