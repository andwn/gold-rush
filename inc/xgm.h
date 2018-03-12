void xgm_init();

void xgm_pcm_set(const uint8_t id, const uint8_t *sample, const uint32_t len);
void xgm_pcm_play(const uint8_t id, const uint8_t priority, const uint16_t channel);

void xgm_vblank();
