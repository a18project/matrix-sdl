#ifndef MATRIX_CONFIG_H
#define MATRIX_CONFIG_H

#include <stdbool.h>

#define GLYPH_WIDTH 20
#define GLYPH_HEIGHT 20
#define NUM_STANDARD_GLYPHS 135
#define NUM_PD_GLYPHS 10
#define NUM_TOTAL_GLYPHS (NUM_STANDARD_GLYPHS + NUM_PD_GLYPHS)
#define NUM_FADES 32

#define SPRITESHEET_COLS 13
#define SPRITESHEET_ROWS 12

typedef enum {
    RENDER_MODE_COLOR,      /* Modern full-color phosphor green with palettes */
    RENDER_MODE_PLAYDATE    /* Authentic Playdate 1-bit dithered monochrome */
} RenderMode;

typedef enum {
    PALETTE_CLASSIC,        /* 1999 The Matrix iconic green */
    PALETTE_RESURRECTIONS,  /* Modern Matrix Resurrections mint green */
    PALETTE_NIGHTMARE,      /* Matrix Reloaded Vampire/Nightmare red */
    PALETTE_PARADISE,       /* First Matrix golden amber */
    PALETTE_TWILIGHT,       /* Cyan & electric blue */
    PALETTE_TERMINAL_WHITE, /* Crisp monochrome terminal */
    PALETTE_COUNT
} PaletteType;

typedef struct {
    const char *name;
    struct {
        unsigned char r, g, b;
    } cursor;               /* Leading droplet cursor color */
    struct {
        unsigned char r, g, b;
    } high;                 /* Bright trail head */
    struct {
        unsigned char r, g, b;
    } mid;                  /* Mid trail */
    struct {
        unsigned char r, g, b;
    } low;                  /* Tail end */
} ColorPalette;

typedef struct {
    int window_width;
    int window_height;
    bool fullscreen;
    bool vsync;
    float fall_speed;
    float glyph_cycle_speed;
    float raindrop_length;
    bool paused;
    RenderMode render_mode;
    PaletteType palette;
    bool glow_effect;
    bool show_hud;
    const char *assets_path;
    int test_frames;        /* If > 0, exit cleanly after N frames (useful for headless testing/CI) */
} AppConfig;

void config_set_defaults(AppConfig *cfg);

#endif /* MATRIX_CONFIG_H */
