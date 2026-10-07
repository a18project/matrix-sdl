#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "renderer.h"

#define DITHER_ITEMS_PER_ROW 64
#define DITHER_TOTAL_ITEMS (NUM_TOTAL_GLYPHS * NUM_FADES)
#define DITHER_ATLAS_WIDTH (DITHER_ITEMS_PER_ROW * GLYPH_WIDTH)
#define DITHER_ATLAS_ROWS ((DITHER_TOTAL_ITEMS + DITHER_ITEMS_PER_ROW - 1) / DITHER_ITEMS_PER_ROW)
#define DITHER_ATLAS_HEIGHT (DITHER_ATLAS_ROWS * GLYPH_HEIGHT)

struct MatrixRenderer {
    SDL_Window *window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *glyph_atlas_color;
    SDL_Texture *glyph_atlas_dither;
    ColorPalette palettes[PALETTE_COUNT];
};

static void init_palettes(MatrixRenderer *r) {
    r->palettes[PALETTE_CLASSIC] = (ColorPalette){
        .name = "Classic Matrix (1999)",
        .cursor = {235, 255, 235},
        .high   = {140, 255, 140},
        .mid    = {0, 255, 65},
        .low    = {0, 65, 20}
    };
    r->palettes[PALETTE_RESURRECTIONS] = (ColorPalette){
        .name = "Resurrections (2021)",
        .cursor = {240, 255, 250},
        .high   = {120, 255, 210},
        .mid    = {0, 220, 160},
        .low    = {0, 60, 45}
    };
    r->palettes[PALETTE_NIGHTMARE] = (ColorPalette){
        .name = "Nightmare (Reloaded)",
        .cursor = {255, 235, 235},
        .high   = {255, 110, 110},
        .mid    = {230, 25, 25},
        .low    = {70, 5, 5}
    };
    r->palettes[PALETTE_PARADISE] = (ColorPalette){
        .name = "Paradise (Golden)",
        .cursor = {255, 255, 225},
        .high   = {255, 215, 100},
        .mid    = {230, 155, 20},
        .low    = {70, 40, 5}
    };
    r->palettes[PALETTE_TWILIGHT] = (ColorPalette){
        .name = "Twilight (Cyberpunk)",
        .cursor = {230, 245, 255},
        .high   = {100, 210, 255},
        .mid    = {0, 140, 255},
        .low    = {10, 30, 75}
    };
    r->palettes[PALETTE_TERMINAL_WHITE] = (ColorPalette){
        .name = "Terminal (Monochrome)",
        .cursor = {255, 255, 255},
        .high   = {220, 220, 220},
        .mid    = {150, 150, 150},
        .low    = {50, 50, 50}
    };
}

static bool resolve_asset_path(const char *specified_path, const char *filename, char *out_buf, size_t out_size) {
    if (specified_path) {
        snprintf(out_buf, out_size, "%s/%s", specified_path, filename);
        FILE *f = fopen(out_buf, "rb");
        if (f) { fclose(f); return true; }
    }

    const char *candidates[] = {
        "assets",
        "../assets",
        "../../assets",
        "./",
        NULL
    };

    for (int i = 0; candidates[i]; i++) {
        snprintf(out_buf, out_size, "%s/%s", candidates[i], filename);
        FILE *f = fopen(out_buf, "rb");
        if (f) { fclose(f); return true; }
    }

    char *base_path = SDL_GetBasePath();
    if (base_path) {
        snprintf(out_buf, out_size, "%sassets/%s", base_path, filename);
        FILE *f = fopen(out_buf, "rb");
        if (f) { fclose(f); SDL_free(base_path); return true; }

        snprintf(out_buf, out_size, "%s../assets/%s", base_path, filename);
        f = fopen(out_buf, "rb");
        if (f) { fclose(f); SDL_free(base_path); return true; }
        SDL_free(base_path);
    }

    snprintf(out_buf, out_size, "assets/%s", filename);
    return false;
}

static SDL_Texture *build_color_atlas(SDL_Renderer *renderer, const unsigned char *glyphs_rgba, int gw, int gh) {
    /* Create RGBA32 surface from raw glyph image */
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, gw, gh, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        fprintf(stderr, "Error creating surface for color atlas: %s\n", SDL_GetError());
        return NULL;
    }

    Uint32 *pixels = (Uint32 *)surface->pixels;
    for (int y = 0; y < gh; y++) {
        for (int x = 0; x < gw; x++) {
            int src_idx = (y * gw + x) * 4;
            unsigned char r = glyphs_rgba[src_idx];
            unsigned char g = glyphs_rgba[src_idx + 1];
            unsigned char b = glyphs_rgba[src_idx + 2];
            int brightness = (int)r + (int)g + (int)b;

            if (brightness > 60) {
                /* Solid white pixel with full alpha for modulation */
                pixels[y * gw + x] = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
            } else {
                /* Transparent background */
                pixels[y * gw + x] = SDL_MapRGBA(surface->format, 0, 0, 0, 0);
            }
        }
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (tex) {
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    }
    return tex;
}

static SDL_Texture *build_dither_atlas(
    SDL_Renderer *renderer,
    const unsigned char *glyphs_rgba, int gw, int gh,
    const unsigned char *grad_rgba, int grad_w, int grad_h
) {
    (void)gh;
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
        0, DITHER_ATLAS_WIDTH, DITHER_ATLAS_HEIGHT, 32, SDL_PIXELFORMAT_RGBA32
    );
    if (!surface) {
        fprintf(stderr, "Error creating surface for dither atlas: %s\n", SDL_GetError());
        return NULL;
    }

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch_pixels = surface->pitch / 4;

    /* Fill initially with transparent pixels */
    memset(surface->pixels, 0, surface->pitch * surface->h);

    for (int i = 0; i < NUM_TOTAL_GLYPHS; i++) {
        int glyph_col = i % SPRITESHEET_COLS;
        int glyph_row = i / SPRITESHEET_COLS;
        int base_src_x = glyph_col * GLYPH_WIDTH;
        int base_src_y = glyph_row * GLYPH_HEIGHT;

        for (int j = 0; j < NUM_FADES; j++) {
            float fade = (float)j / (float)(NUM_FADES - 1);
            int dx = (int)roundf(fade * (float)(GLYPH_WIDTH - grad_w));

            int item_idx = i * NUM_FADES + j;
            int atlas_col = item_idx % DITHER_ITEMS_PER_ROW;
            int atlas_row = item_idx / DITHER_ITEMS_PER_ROW;
            int dst_base_x = atlas_col * GLYPH_WIDTH;
            int dst_base_y = atlas_row * GLYPH_HEIGHT;

            for (int gy = 0; gy < GLYPH_HEIGHT; gy++) {
                for (int gx = 0; gx < GLYPH_WIDTH; gx++) {
                    int src_x = base_src_x + gx;
                    int src_y = base_src_y + gy;
                    int g_idx = (src_y * gw + src_x) * 4;
                    int g_bright = (int)glyphs_rgba[g_idx] + (int)glyphs_rgba[g_idx + 1] + (int)glyphs_rgba[g_idx + 2];

                    if (g_bright <= 60) {
                        continue; /* Background pixel */
                    }

                    /* Test dither gradient mask at this position */
                    int mask_x = gx - dx;
                    bool blocked = false;
                    if (mask_x >= 0 && mask_x < grad_w && gy < grad_h) {
                        int m_idx = (gy * grad_w + mask_x) * 4;
                        unsigned char alpha = grad_rgba[m_idx + 3];
                        if (alpha > 128) {
                            blocked = true;
                        }
                    } else if (mask_x >= grad_w) {
                        blocked = true;
                    }

                    if (!blocked) {
                        int out_x = dst_base_x + gx;
                        int out_y = dst_base_y + gy;
                        pixels[out_y * pitch_pixels + out_x] = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
                    }
                }
            }
        }
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (tex) {
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    }
    return tex;
}

MatrixRenderer *matrix_renderer_create(SDL_Window *window, SDL_Renderer *sdl_renderer, const char *assets_dir) {
    MatrixRenderer *r = calloc(1, sizeof(MatrixRenderer));
    if (!r) return NULL;

    r->window = window;
    r->sdl_renderer = sdl_renderer;
    init_palettes(r);

    char glyphs_path[1024];
    char gradient_path[1024];
    resolve_asset_path(assets_dir, "matrix-glyphs.png", glyphs_path, sizeof(glyphs_path));
    resolve_asset_path(assets_dir, "fade-gradient.png", gradient_path, sizeof(gradient_path));

    int gw = 0, gh = 0, gch = 0;
    unsigned char *glyphs_rgba = stbi_load(glyphs_path, &gw, &gh, &gch, 4);
    if (!glyphs_rgba) {
        fprintf(stderr, "Failed to load glyphs image from: %s\n", glyphs_path);
        free(r);
        return NULL;
    }

    int grad_w = 0, grad_h = 0, grad_ch = 0;
    unsigned char *grad_rgba = stbi_load(gradient_path, &grad_w, &grad_h, &grad_ch, 4);
    if (!grad_rgba) {
        fprintf(stderr, "Failed to load fade gradient from: %s\n", gradient_path);
        stbi_image_free(glyphs_rgba);
        free(r);
        return NULL;
    }

    r->glyph_atlas_color = build_color_atlas(sdl_renderer, glyphs_rgba, gw, gh);
    r->glyph_atlas_dither = build_dither_atlas(sdl_renderer, glyphs_rgba, gw, gh, grad_rgba, grad_w, grad_h);

    stbi_image_free(glyphs_rgba);
    stbi_image_free(grad_rgba);

    if (!r->glyph_atlas_color || !r->glyph_atlas_dither) {
        matrix_renderer_destroy(r);
        return NULL;
    }

    return r;
}

void matrix_renderer_destroy(MatrixRenderer *r) {
    if (!r) return;
    if (r->glyph_atlas_color) SDL_DestroyTexture(r->glyph_atlas_color);
    if (r->glyph_atlas_dither) SDL_DestroyTexture(r->glyph_atlas_dither);
    free(r);
}

const ColorPalette *matrix_renderer_get_palette(const MatrixRenderer *r, PaletteType type) {
    if (!r || type < 0 || type >= PALETTE_COUNT) return NULL;
    return &r->palettes[type];
}

static inline unsigned char lerp_u8(unsigned char a, unsigned char b, float t) {
    return (unsigned char)((float)a + ((float)b - (float)a) * t);
}

void matrix_renderer_render(MatrixRenderer *r, const MatrixGrid *grid, const AppConfig *cfg) {
    if (!r || !grid || !cfg) return;

    SDL_SetRenderDrawColor(r->sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(r->sdl_renderer);

    int win_w = 0, win_h = 0;
    SDL_GetRendererOutputSize(r->sdl_renderer, &win_w, &win_h);

    float cell_w = (float)win_w / (float)grid->num_columns;
    float cell_h = (float)win_h / (float)grid->num_rows;

    const ColorPalette *pal = &r->palettes[cfg->palette];

    if (cfg->render_mode == RENDER_MODE_COLOR) {
        for (int i = 0; i < grid->num_cells; i++) {
            const MatrixCell *cell = &grid->cells[i];
            float b = cell->brightness;

            if (b <= 0.05f && !cell->is_cursor) {
                continue;
            }

            int col = cell->glyph_index % SPRITESHEET_COLS;
            int row = cell->glyph_index / SPRITESHEET_COLS;
            SDL_Rect src_rect = {
                col * GLYPH_WIDTH,
                row * GLYPH_HEIGHT,
                GLYPH_WIDTH,
                GLYPH_HEIGHT
            };

            SDL_Rect dst_rect = {
                (int)roundf((float)cell->x * cell_w),
                (int)roundf((float)cell->y * cell_h),
                (int)ceilf(cell_w),
                (int)ceilf(cell_h)
            };

            if (cell->is_cursor) {
                /* Cursor head: intense white/bright highlight */
                SDL_SetTextureColorMod(r->glyph_atlas_color, pal->cursor.r, pal->cursor.g, pal->cursor.b);
                SDL_SetTextureAlphaMod(r->glyph_atlas_color, 255);
                SDL_RenderCopy(r->sdl_renderer, r->glyph_atlas_color, &src_rect, &dst_rect);

                if (cfg->glow_effect) {
                    /* Subtle additive halo for the cursor */
                    SDL_SetTextureBlendMode(r->glyph_atlas_color, SDL_BLENDMODE_ADD);
                    SDL_SetTextureAlphaMod(r->glyph_atlas_color, 120);
                    SDL_Rect glow_rect = {
                        dst_rect.x - 2, dst_rect.y - 2,
                        dst_rect.w + 4, dst_rect.h + 4
                    };
                    SDL_RenderCopy(r->sdl_renderer, r->glyph_atlas_color, &src_rect, &glow_rect);
                    SDL_SetTextureBlendMode(r->glyph_atlas_color, SDL_BLENDMODE_BLEND);
                }
            } else {
                /* Trail: smoothly interpolate colors */
                unsigned char red, green, blue, alpha;
                if (b > 0.65f) {
                    float t = (b - 0.65f) / 0.35f;
                    red   = lerp_u8(pal->mid.r, pal->high.r, t);
                    green = lerp_u8(pal->mid.g, pal->high.g, t);
                    blue  = lerp_u8(pal->mid.b, pal->high.b, t);
                    alpha = lerp_u8(210, 255, t);
                } else if (b > 0.20f) {
                    float t = (b - 0.20f) / 0.45f;
                    red   = lerp_u8(pal->low.r, pal->mid.r, t);
                    green = lerp_u8(pal->low.g, pal->mid.g, t);
                    blue  = lerp_u8(pal->low.b, pal->mid.b, t);
                    alpha = lerp_u8(110, 210, t);
                } else {
                    float t = (b - 0.05f) / 0.15f;
                    red   = lerp_u8(0, pal->low.r, t);
                    green = lerp_u8(0, pal->low.g, t);
                    blue  = lerp_u8(0, pal->low.b, t);
                    alpha = lerp_u8(20, 110, t);
                }

                SDL_SetTextureColorMod(r->glyph_atlas_color, red, green, blue);
                SDL_SetTextureAlphaMod(r->glyph_atlas_color, alpha);
                SDL_RenderCopy(r->sdl_renderer, r->glyph_atlas_color, &src_rect, &dst_rect);
            }
        }
    } else {
        /* Playdate 1-Bit Retro Dither Mode */
        SDL_SetTextureColorMod(r->glyph_atlas_dither, 255, 255, 255);
        SDL_SetTextureAlphaMod(r->glyph_atlas_dither, 255);

        for (int i = 0; i < grid->num_cells; i++) {
            const MatrixCell *cell = &grid->cells[i];
            /* Map brightness to dither step: 1.0 (bright) -> 0 (visible); 0.0 (dark) -> 31 (masked) */
            int dither_step = (int)((1.0f - cell->brightness) * (float)(NUM_FADES - 1));
            if (dither_step < 0) dither_step = 0;
            if (dither_step >= NUM_FADES - 1) {
                continue; /* Fully masked out to black */
            }

            int item_idx = cell->glyph_index * NUM_FADES + dither_step;
            int atlas_col = item_idx % DITHER_ITEMS_PER_ROW;
            int atlas_row = item_idx / DITHER_ITEMS_PER_ROW;

            SDL_Rect src_rect = {
                atlas_col * GLYPH_WIDTH,
                atlas_row * GLYPH_HEIGHT,
                GLYPH_WIDTH,
                GLYPH_HEIGHT
            };

            SDL_Rect dst_rect = {
                (int)roundf((float)cell->x * cell_w),
                (int)roundf((float)cell->y * cell_h),
                (int)ceilf(cell_w),
                (int)ceilf(cell_h)
            };

            SDL_RenderCopy(r->sdl_renderer, r->glyph_atlas_dither, &src_rect, &dst_rect);
        }
    }

    SDL_RenderPresent(r->sdl_renderer);
}
