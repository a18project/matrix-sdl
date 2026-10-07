#ifndef MATRIX_RENDERER_H
#define MATRIX_RENDERER_H

#include <SDL.h>
#include "config.h"
#include "matrix.h"

typedef struct MatrixRenderer MatrixRenderer;

MatrixRenderer *matrix_renderer_create(SDL_Window *window, SDL_Renderer *sdl_renderer, const char *assets_dir);
void matrix_renderer_destroy(MatrixRenderer *r);
void matrix_renderer_render(MatrixRenderer *r, const MatrixGrid *grid, const AppConfig *cfg);
const ColorPalette *matrix_renderer_get_palette(const MatrixRenderer *r, PaletteType type);

#endif /* MATRIX_RENDERER_H */
