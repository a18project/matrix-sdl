#ifndef MATRIX_GUI_H
#define MATRIX_GUI_H

#include <SDL.h>
#include <stdbool.h>
#include "config.h"
#include "matrix.h"

void gui_init(void);
void gui_open(AppConfig *cfg);
void gui_close(AppConfig *cfg);
void gui_toggle(AppConfig *cfg);

/* Returns true if the event was captured/consumed by the GUI */
bool gui_handle_event(const SDL_Event *ev, AppConfig *cfg, MatrixGrid *grid, SDL_Window *window);

/* Render GUI overlay */
void gui_render(SDL_Renderer *renderer, const AppConfig *cfg, const MatrixGrid *grid);

#endif /* MATRIX_GUI_H */
