#include "common.h"
#include "vdp.h"
#include "tiles_res.h"
#include "resources.h"

static const uint16_t blank_data[0x80];

static volatile uint16_t* const vdp_data_port = (uint16_t*) 0xC00000;
static volatile uint16_t* const vdp_ctrl_port = (uint16_t*) 0xC00004;
static volatile uint32_t* const vdp_ctrl_wide = (uint32_t*) 0xC00004;

static uint16_t spr_count;
static VDPSprite spr_table[80];

void vdp_init() {
	// Set the registers
	*vdp_ctrl_port = 0x8004;
	*vdp_ctrl_port = 0x8174; // Enable display
	*vdp_ctrl_port = 0x8200 | (VDP_PLAN_A >> 10); // Plane A address
	*vdp_ctrl_port = 0x8300 | (VDP_PLAN_W >> 10); // Window address
	*vdp_ctrl_port = 0x8400 | (VDP_PLAN_B >> 13); // Plane B address
	*vdp_ctrl_port = 0x8500 | (VDP_SPRITE_TABLE >> 9); // Sprite list address
	*vdp_ctrl_port = 0x8600;
	*vdp_ctrl_port = 0x8700; // Background color palette index
	*vdp_ctrl_port = 0x8800;
	*vdp_ctrl_port = 0x8900;
	*vdp_ctrl_port = 0x8A01; // Horizontal interrupt timer
	*vdp_ctrl_port = 0x8B00 | (VSCROLL_PLANE << 2) | HSCROLL_PLANE; // Scroll mode
	*vdp_ctrl_port = 0x8C81; // No interlace or shadow/highlight
	*vdp_ctrl_port = 0x8D00 | (VDP_HSCROLL_TABLE >> 10); // HScroll table address
	*vdp_ctrl_port = 0x8E00;
	*vdp_ctrl_port = 0x8F02; // Auto increment
	*vdp_ctrl_port = 0x9001; // Map size (64x32)
	*vdp_ctrl_port = 0x9100; // Window X
	*vdp_ctrl_port = 0x9200; // Window Y
	
	pal_mode = *vdp_ctrl_port & 1;
}

void vdp_reset() {
	// Blank out first tile
	vdp_load_tiles(blank_data, 0, 1);
	// Reset the tilemaps
	int16_t hscroll[32] = {};
	vdp_map_clear(VDP_PLAN_A);
	vdp_set_hscroll(VDP_PLAN_A, hscroll);
	vdp_set_vscroll(VDP_PLAN_A, 0);
	vdp_map_clear(VDP_PLAN_B);
	vdp_set_hscroll(VDP_PLAN_B, hscroll);
	vdp_set_vscroll(VDP_PLAN_B, 0);
	// Reset sprites
	vdp_sprites_clear();
	vdp_sprites_update();
	// (Re)load the font
	vdp_load_font();
	vdp_set_color(1, 0x000);
	vdp_set_color(15, 0xEEE);
}

// Register stuff

void vdp_set_display(uint8_t enabled) {
	*vdp_ctrl_port = 0x8134 | (enabled << 6);
}

void vdp_set_autoinc(uint8_t val) {
	*vdp_ctrl_port = 0x8F00 | val;
}

void vdp_set_scrollmode(uint8_t hoz, uint8_t vert) {
	*vdp_ctrl_port = 0x8B00 | (vert << 2) | hoz;
}

void vdp_set_highlight(uint8_t enabled) {
	*vdp_ctrl_port = 0x8C81 | (enabled << 3);
}

void vdp_vsync() {
	while(*vdp_ctrl_port & 8);
	while(!(*vdp_ctrl_port & 8));
}

// DMA stuff

static void dma_do(uint32_t from, uint16_t len, uint32_t cmd) {
	// Setup DMA length (in word here)
    *vdp_ctrl_port = 0x9300 + (len & 0xff);
    *vdp_ctrl_port = 0x9400 + ((len >> 8) & 0xff);
    // Setup DMA address
    from >>= 1;
    *vdp_ctrl_port = 0x9500 + (from & 0xff);
    from >>= 8;
    *vdp_ctrl_port = 0x9600 + (from & 0xff);
    from >>= 8;
    *vdp_ctrl_port = 0x9700 + (from & 0x7f);
    // Enable DMA transfer
    *vdp_ctrl_wide = cmd;
}

void vdp_dma_vram(uint32_t from, uint16_t to, uint16_t len) {
	dma_do(from, len, ((0x4000 + ((to) & 0x3FFF)) << 16) + (((to) >> 14) | 0x80));
}

void vdp_dma_cram(uint32_t from, uint16_t to, uint16_t len) {
	dma_do(from, len, ((0xC000 + ((to) & 0x3FFF)) << 16) + (((to) >> 14) | 0x80));
}

void vdp_dma_vsram(uint32_t from, uint16_t to, uint16_t len) {
	dma_do(from, len, ((0x4000 + ((to) & 0x3FFF)) << 16) + (((to) >> 14) | 0x90));
}

// Tile patterns

void vdp_load_tiles(const void *data, uint16_t index, uint16_t num) {
	vdp_dma_vram((uint32_t) data, index << 5, num << 4);
}

// Tile maps

void vdp_map_set_xy(uint16_t plan, uint16_t tile, uint16_t x, uint16_t y) {
    uint16_t addr = plan + ((x + (y << PLAN_WIDTH_SFT)) << 1);
    *vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x00);
	*vdp_data_port = tile;
}

void vdp_map_set_hline(uint16_t plan, const uint16_t *tiles, uint16_t x, uint16_t y, uint16_t len) {
	uint16_t addr = plan + ((x + (y << PLAN_WIDTH_SFT)) << 1);
	if(len > 64) len = 64;
	vdp_dma_vram((uint32_t) tiles, addr, len);
}

void vdp_map_set_vline(uint16_t plan, const uint16_t *tiles, uint16_t x, uint16_t y, uint16_t len) {
	uint16_t addr = plan + ((x + (y << PLAN_WIDTH_SFT)) << 1);
	if(len > 32) len = 32;
	vdp_set_autoinc(128);
	vdp_dma_vram((uint32_t) tiles, addr, len);
	vdp_set_autoinc(2);
}

void vdp_map_clear(uint16_t plan) {
	uint16_t addr = plan;
	while(addr < plan + 0x1000) {
		vdp_dma_vram((uint32_t) blank_data, addr, 0x80);
		addr += 0x100;
	}
}

// Palettes

void vdp_set_colors(uint16_t index, const void *values, uint16_t count) {
	vdp_dma_cram((uint32_t) values, index << 1, count);
}

void vdp_set_color(uint16_t index, uint16_t color) {
    index <<= 1;
    *vdp_ctrl_wide = ((0xC000 + ((index) & 0x3FFF)) << 16) + (((index) >> 14) | 0x00);
    *vdp_data_port = color;
}

// Scroll

void vdp_set_hscroll(uint16_t plan, const int16_t *hscroll) {
	uint16_t addr = (plan == VDP_PLAN_A) ? VDP_HSCROLL_TABLE : VDP_HSCROLL_TABLE + 2;
	vdp_set_autoinc(32);
	vdp_dma_vram((uint32_t) hscroll, addr, 32);
	vdp_set_autoinc(2);
}

void vdp_set_vscroll(uint16_t plan, int16_t vscroll) {
	uint16_t addr = (plan == VDP_PLAN_A) ? 0 : 2;
	*vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x10);
	*vdp_data_port = vscroll;
}

// Sprites

void vdp_sprites_add(const VDPSprite *spr, uint16_t num) {
	for(uint16_t i = 0; i < num; i++) {
		// Exceeded max number of sprites
		if(spr_count >= 80) break;
		// Prevent drawing off screen sprites
		if((unsigned)(spr[i].x-96) < 352 && (unsigned)(spr[i].y-96) < 256) {
			spr_table[spr_count] = spr[i];
			spr_table[spr_count].link = spr_count + 1;
			spr_count++;
		}
	}
}

void vdp_sprites_clear() {
	static const VDPSprite NULL_SPRITE = {
		.x = 128, .y = 128, .size = SPRITE_SIZE(1,1), .attr = 0
	};
	spr_count = 0;
	vdp_sprites_add(&NULL_SPRITE, 1);
}

void vdp_sprites_update() {
	if(!spr_count) return;
	spr_table[spr_count - 1].link = 0; // Mark end of sprite list
	vdp_dma_vram((uint32_t) spr_table, VDP_SPRITE_TABLE, spr_count << 2);
	spr_count = 0;
}

// Font / Text

void vdp_load_font() {
	vdp_load_tiles(PAT_Font, TILE_FONTINDEX, 0x60);
}

void vdp_puts(uint16_t plan, const char *str, uint16_t x, uint16_t y) {
	uint16_t addr = plan + ((x + (y << PLAN_WIDTH_SFT)) << 1);
	*vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x00);
	
	for(uint16_t i = 0; i < 64 && *str; ++i) {
		// Wrap around the plane, don't fall to next line
		if(i + x == 64) {
			addr -= x << 1;
			*vdp_ctrl_wide = ((0x4000 + ((addr) & 0x3FFF)) << 16) + (((addr) >> 14) | 0x00);
		}
		uint16_t attr = TILE_ATTR(0,1,0,0,TILE_FONTINDEX + *str++ - 0x20);
		*vdp_data_port = attr;
	}
}
