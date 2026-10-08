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
    RENDER_MODE_COLOR,      /* Modern full-color phosphor rain with palettes & effects */
    RENDER_MODE_PLAYDATE    /* Authentic Playdate 1-bit dithered monochrome */
} RenderMode;

typedef enum {
    VERSION_CLASSIC,        /* 1999/2003 Iconic green code rain */
    VERSION_RESURRECTIONS,  /* 2021 Matrix Resurrections mint code */
    VERSION_OPERATOR,       /* 1999 Operator monitors & opening title */
    VERSION_NIGHTMARE,      /* Merovingian vampire/werewolf crimson red */
    VERSION_PARADISE,       /* First idyllic Matrix golden amber */
    VERSION_PALIMPSEST,     /* Rob Dougan Furious Angels teal/gold */
    VERSION_TWILIGHT,       /* Cyberpunk cyan & neon violet */
    VERSION_MORPHEUS,       /* Deep royal purple & crimson */
    VERSION_TRINITY,        /* Emerald with radiant gold highlights */
    VERSION_BUGS,           /* Electric blue & cyan */
    VERSION_MEGACITY,       /* Revolutions Megacity glyph code */
    VERSION_PLAYDATE,       /* Playdate 1-bit monochrome dither */
    VERSION_COUNT
} MatrixVersion;

typedef enum {
    EFFECT_PALETTE,         /* Standard tone-mapped color palette */
    EFFECT_PRIDE,           /* 6-color rainbow pride flag stripes */
    EFFECT_TRANS_PRIDE,     /* 5-color trans pride flag stripes */
    EFFECT_STRIPES,         /* Cyberpunk cyan/magenta dual stripes */
    EFFECT_COUNT
} MatrixEffect;

typedef enum {
    PALETTE_CLASSIC,        /* 1999 The Matrix iconic phosphor green */
    PALETTE_RESURRECTIONS,  /* Modern Matrix Resurrections mint green */
    PALETTE_OPERATOR,       /* Bright 1999 operator terminal green */
    PALETTE_NIGHTMARE,      /* Matrix Reloaded Vampire/Nightmare red */
    PALETTE_PARADISE,       /* First Matrix golden amber */
    PALETTE_PALIMPSEST,     /* Furious Angels teal & golden amber */
    PALETTE_TWILIGHT,       /* Cyan & electric violet */
    PALETTE_MORPHEUS,       /* Deep violet & crimson */
    PALETTE_TRINITY,        /* Emerald with gold glints */
    PALETTE_BUGS,           /* Electric cerulean & cyan */
    PALETTE_TERMINAL_WHITE, /* Crisp monochrome terminal */
    PALETTE_AMBER_CRT,      /* Vintage monochrome amber CRT */
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
    float slant;            /* Rain slant factor (-0.5 to 0.5) */
    bool paused;
    RenderMode render_mode;
    MatrixVersion version;
    MatrixEffect effect;
    PaletteType palette;
    bool glow_effect;
    bool bonus_glyphs;
    bool settings_gui_open;
    bool show_hud;
    const char *assets_path;
    int test_frames;        /* If > 0, exit cleanly after N frames (testing/CI) */
} AppConfig;

void config_set_defaults(AppConfig *cfg);
void config_apply_version(AppConfig *cfg, MatrixVersion ver);
const char *config_version_name(MatrixVersion ver);
const char *config_version_desc(MatrixVersion ver);
const char *config_effect_name(MatrixEffect eff);

#endif /* MATRIX_CONFIG_H */
