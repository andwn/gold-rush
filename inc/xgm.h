void xgm_init();

void xgm_music_play(const uint8_t *song);
void xgm_music_stop();
void xgm_music_pause();
void xgm_music_resume();

void xgm_pcm_set(const uint8_t id, const uint8_t *sample, const uint32_t len);
void xgm_pcm_play(const uint8_t id, const uint8_t priority, const uint16_t channel);

void xgm_vblank();
void xgm_protect(uint8_t enable);
