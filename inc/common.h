#define FALSE   0
#define TRUE    1
#define NULL    0

// stdint types
typedef signed char int8_t ;
typedef unsigned char uint8_t ;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed int int32_t;
typedef unsigned int uint32_t;

// Rescomp types
typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;

typedef struct {
    u16 length;
    u16 *data;
} Palette;

// Helper to grab tiles from a SpriteDefinition
#define SPR_TILES(spr, a, f) ((spr)->animations[a]->frames[f]->tileset->tiles)
