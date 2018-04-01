extern const uint32_t PAT_Font[16 * 6 * 8];
extern const uint32_t PAT_Money[7 * 7 * 8];
extern const uint32_t PAT_Layout[1696 / 4];
extern const uint32_t PAT_Notes[8 * 8];
extern const uint32_t PAT_Bar[6 * 8];
extern const uint32_t PAT_Words[7936 / 4];
extern const uint32_t PAT_Glow[8 * 8 * 8];
extern const uint32_t PAT_Speed[4 * 2 * 8];

extern const uint32_t PAT_EasyWin[38 * 24 * 8];
extern const uint32_t PAT_EasyLose[24 * 27 * 8];
extern const uint32_t PAT_NormalWin[38 * 22 * 8];
extern const uint32_t PAT_NormalLose[38 * 21 * 8];

#define SIZE_OF_BANK	((uint32_t)(PAT_NormalLose+sizeof(PAT_NormalLose)-PAT_Font))