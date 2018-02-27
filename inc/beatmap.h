#define BEATMAP_LEN 0x400

// Active timers for track
uint16_t c_beat;
uint16_t c_frame;
uint16_t c_subframe;
// Speed at which to increment timers
uint16_t s_frames;
uint16_t s_subframes;

void beatmap_init();
void beatmap_update();
