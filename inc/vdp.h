#define VDP_PLAN_W				0xB000
#define VDP_PLAN_A              0xC000
#define VDP_PLAN_B              0xE000
#define VDP_SPRITE_TABLE        0xF800
#define VDP_HSCROLL_TABLE       0xFC00

#define PLAN_WIDTH				64
#define PLAN_HEIGHT				32
#define PLAN_WIDTH_SFT			6
#define PLAN_HEIGHT_SFT			5

#define HSCROLL_PLANE           0
#define HSCROLL_TILE            2
#define HSCROLL_LINE            3
#define VSCROLL_PLANE           0
#define VSCROLL_2TILE           1

#define TILE_INDEX_MASK         0x7FF

#define TILE_BLANKINDEX         0x0000 // First tile is kept blank/transparent
#define TILE_EXTRA1INDEX 		(0xD000 >> 5) // 128 tiles after PLAN_A
#define TILE_EXTRA2INDEX 		(0xF000 >> 5) // 64 tiles after PLAN_B
#define TILE_EXTRA3INDEX 		(0xFA80 >> 5) // 12 tiles after sprite list

// Game specific tile indeces
#define TILE_LAYOUTINDEX		0x0000
#define TILE_WORDSINDEX			0x0300
#define TILE_FONTINDEX			TILE_EXTRA1INDEX
#define TILE_BARINDEX			(TILE_FONTINDEX + 0x60)
#define TILE_SPEEDINDEX			(TILE_BARINDEX + 6)
#define TILE_GLOWINDEX			TILE_EXTRA2INDEX
#define TILE_NOTEINDEX			TILE_EXTRA3INDEX

#define TILE_ATTR(pal, prio, flipV, flipH, index)                                              \
	(((flipH) << 11) | ((flipV) << 12) | ((pal) << 13) | ((prio) << 15) | (index))

#define sprite_pos(s, px, py) {                                                                \
	(s).x = (px) + 128;                                                                        \
	(s).y = (py) + 128;                                                                        \
}

#define sprite_pri(s, pri)   { (s).attr &= ~(1<<15); (s).attr |= ((pri)&1) << 15; }
#define sprite_pal(s, pal)   { (s).attr &= ~(3<<13); (s).attr |= ((pal)&3) << 13; }
#define sprite_vflip(s, flp) { (s).attr &= ~(1<<12); (s).attr |= ((flp)&1) << 12; }
#define sprite_hflip(s, flp) { (s).attr &= ~(1<<11); (s).attr |= ((flp)&1) << 11; }
#define sprite_index(s, ind) { (s).attr &= ~0x7FF;   (s).attr |= (ind)&0x7FF; }

#define SPRITE_SIZE(w, h) ((((w) - 1) << 2) | ((h) - 1))

typedef struct {
	int16_t y;
	uint8_t size;
	uint8_t link;
    uint16_t attr;
    int16_t x;
} VDPSprite;

uint16_t pal_mode;

void vdp_init();
void vdp_reset();

// Register stuff
void vdp_set_display(uint8_t enabled);
void vdp_set_autoinc(uint8_t val);
void vdp_set_scrollmode(uint8_t hoz, uint8_t vert);
void vdp_set_highlight(uint8_t enabled);

void vdp_vsync();

// DMA stuff
void vdp_dma_vram(uint32_t from, uint16_t to, uint16_t len);
void vdp_dma_cram(uint32_t from, uint16_t to, uint16_t len);
void vdp_dma_vsram(uint32_t from, uint16_t to, uint16_t len);

// Tile patterns
void vdp_load_tiles(const void *data, uint16_t index, uint16_t num);

// Tile maps
void vdp_map_set_xy(uint16_t plan, uint16_t tile, uint16_t x, uint16_t y);
void vdp_map_set_hline(uint16_t plan, const uint16_t *tiles, uint16_t x, uint16_t y, uint16_t len);
void vdp_map_set_vline(uint16_t plan, const uint16_t *tiles, uint16_t x, uint16_t y, uint16_t len);
void vdp_map_clear(uint16_t plan);

// Palettes
void vdp_set_colors(uint16_t index, const void *values, uint16_t count);
void vdp_set_color(uint16_t index, uint16_t color);

// Scrolling
void vdp_set_hscroll(uint16_t plan, const int16_t *scroll);
void vdp_set_vscroll(uint16_t plan, int16_t scroll);

// Sprites
void vdp_sprites_add(const VDPSprite *spr, uint16_t num);
void vdp_sprites_clear();
void vdp_sprites_update();

// Text
void vdp_load_font();
void vdp_puts(uint16_t plan, const char *str, uint16_t x, uint16_t y);
