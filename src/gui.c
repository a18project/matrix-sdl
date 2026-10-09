#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gui.h"
#include "gui_font.h"

#define TAB_COUNT 4

static const char *tab_names[TAB_COUNT] = {
    "1:VERSIONS",
    "2:EFFECTS",
    "3:TUNING",
    "4:ABOUT"
};

static int current_tab = 0;
static int selected_item = 0;
static int mouse_x = 0;
static int mouse_y = 0;

void gui_init(void) {
    current_tab = 0;
    selected_item = 0;
}

void gui_open(AppConfig *cfg) {
    if (cfg) cfg->settings_gui_open = true;
}

void gui_close(AppConfig *cfg) {
    if (cfg) cfg->settings_gui_open = false;
}

void gui_toggle(AppConfig *cfg) {
    if (cfg) cfg->settings_gui_open = !cfg->settings_gui_open;
}

static void draw_char(SDL_Renderer *r, int x, int y, char c, SDL_Color color, int scale) {
    if (c < 32 || c > 126) c = '?';
    const uint8_t *bitmap = gui_font_data[c - 32];
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);

    for (int row = 0; row < 8; row++) {
        uint8_t bits = bitmap[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                if (scale == 1) {
                    SDL_RenderDrawPoint(r, x + col, y + row);
                } else {
                    SDL_Rect rect = { x + col * scale, y + row * scale, scale, scale };
                    SDL_RenderFillRect(r, &rect);
                }
            }
        }
    }
}

static void draw_text(SDL_Renderer *r, int x, int y, const char *str, SDL_Color color, int scale) {
    if (!str) return;
    int cur_x = x;
    int char_w = 8 * scale;
    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            y += 10 * scale;
        } else {
            draw_char(r, cur_x, y, *str, color, scale);
            cur_x += char_w;
        }
        str++;
    }
}

static void draw_rect(SDL_Renderer *r, int x, int y, int w, int h, SDL_Color color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderDrawRect(r, &rect);
}

static void fill_rect(SDL_Renderer *r, int x, int y, int w, int h, SDL_Color color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(r, &rect);
}

static bool is_point_in_rect(int px, int py, int rx, int ry, int rw, int rh) {
    return (px >= rx && px < rx + rw && py >= ry && py < ry + rh);
}

static void sync_grid_with_config(MatrixGrid *grid, const AppConfig *cfg) {
    if (!grid || !cfg) return;
    grid->fall_speed = cfg->fall_speed;
    grid->raindrop_length = cfg->raindrop_length;
    grid->glyph_cycle_speed = cfg->glyph_cycle_speed;
    grid->slant = cfg->slant;
    grid->forward_speed = cfg->forward_speed;
    grid->volumetric = cfg->volumetric;
    grid->bonus_glyphs = cfg->bonus_glyphs;
}

bool gui_handle_event(const SDL_Event *ev, AppConfig *cfg, MatrixGrid *grid, SDL_Window *window) {
    if (!cfg || !grid) return false;

    /* Always allow Tab / F2 / Gamepad Select to toggle GUI */
    if (ev->type == SDL_KEYDOWN) {
        if (ev->key.keysym.sym == SDLK_TAB || ev->key.keysym.sym == SDLK_F2) {
            gui_toggle(cfg);
            return true;
        }
        if (cfg->settings_gui_open && (ev->key.keysym.sym == SDLK_ESCAPE)) {
            gui_close(cfg);
            return true;
        }
    } else if (ev->type == SDL_CONTROLLERBUTTONDOWN) {
        if (ev->cbutton.button == SDL_CONTROLLER_BUTTON_BACK || ev->cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE) {
            gui_toggle(cfg);
            return true;
        }
        if (cfg->settings_gui_open && ev->cbutton.button == SDL_CONTROLLER_BUTTON_B) {
            gui_close(cfg);
            return true;
        }
    }

    if (!cfg->settings_gui_open) {
        /* If closed and user clicks bottom-right gear badge, open GUI */
        if (ev->type == SDL_MOUSEBUTTONDOWN && ev->button.button == SDL_BUTTON_LEFT) {
            int win_w = 0, win_h = 0;
            SDL_GetWindowSize(window, &win_w, &win_h);
            int badge_w = 140;
            int badge_h = 24;
            int badge_x = win_w - badge_w - 10;
            int badge_y = win_h - badge_h - 10;
            if (is_point_in_rect(ev->button.x, ev->button.y, badge_x, badge_y, badge_w, badge_h)) {
                gui_open(cfg);
                return true;
            }
        }
        return false;
    }

    /* While GUI is open, consume and process navigation events */
    if (ev->type == SDL_MOUSEMOTION) {
        mouse_x = ev->motion.x;
        mouse_y = ev->motion.y;
        return true;
    }

    /* Keyboard navigation */
    if (ev->type == SDL_KEYDOWN) {
        SDL_Keycode k = ev->key.keysym.sym;

        if (k == SDLK_LEFTBRACKET || k == SDLK_PAGEUP) {
            current_tab = (current_tab + TAB_COUNT - 1) % TAB_COUNT;
            selected_item = 0;
            return true;
        }
        if (k == SDLK_RIGHTBRACKET || k == SDLK_PAGEDOWN) {
            current_tab = (current_tab + 1) % TAB_COUNT;
            selected_item = 0;
            return true;
        }

        if (current_tab == 0) {
            /* TAB 0: VERSIONS */
            if (k == SDLK_UP) {
                selected_item = (selected_item + VERSION_COUNT - 1) % VERSION_COUNT;
            } else if (k == SDLK_DOWN) {
                selected_item = (selected_item + 1) % VERSION_COUNT;
            } else if (k == SDLK_LEFT && selected_item >= 7) {
                selected_item -= 7;
            } else if (k == SDLK_RIGHT && selected_item < 7) {
                selected_item = (selected_item + 7 < VERSION_COUNT) ? (selected_item + 7) : (VERSION_COUNT - 1);
            } else if (k == SDLK_RETURN || k == SDLK_SPACE) {
                config_apply_version(cfg, (MatrixVersion)selected_item);
                sync_grid_with_config(grid, cfg);
            }
            return true;
        } else if (current_tab == 1) {
            /* TAB 1: EFFECTS */
            if (k == SDLK_UP) {
                selected_item = (selected_item + 5) % 6;
            } else if (k == SDLK_DOWN) {
                selected_item = (selected_item + 1) % 6;
            } else if (k == SDLK_LEFT || k == SDLK_RIGHT || k == SDLK_RETURN || k == SDLK_SPACE) {
                int delta = (k == SDLK_LEFT) ? -1 : 1;
                switch (selected_item) {
                    case 0:
                        cfg->effect = (MatrixEffect)((cfg->effect + delta + EFFECT_COUNT) % EFFECT_COUNT);
                        break;
                    case 1:
                        cfg->palette = (PaletteType)((cfg->palette + delta + PALETTE_COUNT) % PALETTE_COUNT);
                        break;
                    case 2:
                        cfg->glow_effect = !cfg->glow_effect;
                        break;
                    case 3:
                        cfg->bonus_glyphs = !cfg->bonus_glyphs;
                        grid->bonus_glyphs = cfg->bonus_glyphs;
                        break;
                    case 4:
                        cfg->render_mode = (cfg->render_mode == RENDER_MODE_COLOR) ? RENDER_MODE_PLAYDATE : RENDER_MODE_COLOR;
                        break;
                    case 5:
                        cfg->volumetric = !cfg->volumetric;
                        grid->volumetric = cfg->volumetric;
                        break;
                }
            }
            return true;
        } else if (current_tab == 2) {
            /* TAB 2: TUNING */
            if (k == SDLK_UP) {
                selected_item = (selected_item + 7) % 8;
            } else if (k == SDLK_DOWN) {
                selected_item = (selected_item + 1) % 8;
            } else if (k == SDLK_LEFT || k == SDLK_RIGHT) {
                float dir = (k == SDLK_LEFT) ? -1.0f : 1.0f;
                switch (selected_item) {
                    case 0:
                        cfg->fall_speed += dir * 0.10f;
                        if (cfg->fall_speed < 0.10f) cfg->fall_speed = 0.10f;
                        if (cfg->fall_speed > 4.00f) cfg->fall_speed = 4.00f;
                        grid->fall_speed = cfg->fall_speed;
                        break;
                    case 1:
                        cfg->raindrop_length += dir * 0.10f;
                        if (cfg->raindrop_length < 0.30f) cfg->raindrop_length = 0.30f;
                        if (cfg->raindrop_length > 3.00f) cfg->raindrop_length = 3.00f;
                        grid->raindrop_length = cfg->raindrop_length;
                        break;
                    case 2:
                        cfg->glyph_cycle_speed += dir * 0.20f;
                        if (cfg->glyph_cycle_speed < 0.40f) cfg->glyph_cycle_speed = 0.40f;
                        if (cfg->glyph_cycle_speed > 5.00f) cfg->glyph_cycle_speed = 5.00f;
                        grid->glyph_cycle_speed = cfg->glyph_cycle_speed;
                        break;
                    case 3:
                        cfg->forward_speed += dir * 0.05f;
                        if (cfg->forward_speed < 0.05f) cfg->forward_speed = 0.05f;
                        if (cfg->forward_speed > 2.00f) cfg->forward_speed = 2.00f;
                        grid->forward_speed = cfg->forward_speed;
                        break;
                    case 4:
                        if (cfg->slant == 0.0f) cfg->slant = (dir > 0) ? 0.35f : -0.25f;
                        else if (cfg->slant > 0.0f) cfg->slant = (dir > 0) ? -0.25f : 0.0f;
                        else cfg->slant = (dir > 0) ? 0.0f : 0.35f;
                        grid->slant = cfg->slant;
                        break;
                }
            } else if (k == SDLK_RETURN || k == SDLK_SPACE) {
                if (selected_item == 5) {
                    cfg->paused = !cfg->paused;
                } else if (selected_item == 6) {
                    cfg->fullscreen = !cfg->fullscreen;
                    SDL_SetWindowFullscreen(window, cfg->fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                } else if (selected_item == 7) {
                    config_set_defaults(cfg);
                    sync_grid_with_config(grid, cfg);
                }
            }
            return true;
        } else if (current_tab == 3) {
            /* TAB 3: ABOUT */
            if (k == SDLK_RETURN || k == SDLK_SPACE) {
                gui_close(cfg);
            }
            return true;
        }
    }

    /* Gamepad navigation */
    if (ev->type == SDL_CONTROLLERBUTTONDOWN) {
        SDL_GameControllerButton btn = ev->cbutton.button;
        if (btn == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) {
            current_tab = (current_tab + TAB_COUNT - 1) % TAB_COUNT;
            selected_item = 0;
            return true;
        }
        if (btn == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
            current_tab = (current_tab + 1) % TAB_COUNT;
            selected_item = 0;
            return true;
        }

        if (current_tab == 0) {
            if (btn == SDL_CONTROLLER_BUTTON_DPAD_UP) {
                selected_item = (selected_item + VERSION_COUNT - 1) % VERSION_COUNT;
            } else if (btn == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
                selected_item = (selected_item + 1) % VERSION_COUNT;
            } else if (btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT && selected_item >= 7) {
                selected_item -= 7;
            } else if (btn == SDL_CONTROLLER_BUTTON_DPAD_RIGHT && selected_item < 7) {
                selected_item = (selected_item + 7 < VERSION_COUNT) ? (selected_item + 7) : (VERSION_COUNT - 1);
            } else if (btn == SDL_CONTROLLER_BUTTON_A || btn == SDL_CONTROLLER_BUTTON_START) {
                config_apply_version(cfg, (MatrixVersion)selected_item);
                sync_grid_with_config(grid, cfg);
            }
            return true;
        } else if (current_tab == 1) {
            if (btn == SDL_CONTROLLER_BUTTON_DPAD_UP) selected_item = (selected_item + 5) % 6;
            else if (btn == SDL_CONTROLLER_BUTTON_DPAD_DOWN) selected_item = (selected_item + 1) % 6;
            else if (btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT || btn == SDL_CONTROLLER_BUTTON_DPAD_RIGHT || btn == SDL_CONTROLLER_BUTTON_A) {
                int delta = (btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT) ? -1 : 1;
                switch (selected_item) {
                    case 0: cfg->effect = (MatrixEffect)((cfg->effect + delta + EFFECT_COUNT) % EFFECT_COUNT); break;
                    case 1: cfg->palette = (PaletteType)((cfg->palette + delta + PALETTE_COUNT) % PALETTE_COUNT); break;
                    case 2: cfg->glow_effect = !cfg->glow_effect; break;
                    case 3: cfg->bonus_glyphs = !cfg->bonus_glyphs; grid->bonus_glyphs = cfg->bonus_glyphs; break;
                    case 4: cfg->render_mode = (cfg->render_mode == RENDER_MODE_COLOR) ? RENDER_MODE_PLAYDATE : RENDER_MODE_COLOR; break;
                    case 5: cfg->volumetric = !cfg->volumetric; grid->volumetric = cfg->volumetric; break;
                }
            }
            return true;
        } else if (current_tab == 2) {
            if (btn == SDL_CONTROLLER_BUTTON_DPAD_UP) selected_item = (selected_item + 7) % 8;
            else if (btn == SDL_CONTROLLER_BUTTON_DPAD_DOWN) selected_item = (selected_item + 1) % 8;
            else if (btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT || btn == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
                float dir = (btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT) ? -1.0f : 1.0f;
                switch (selected_item) {
                    case 0:
                        cfg->fall_speed += dir * 0.10f;
                        if (cfg->fall_speed < 0.10f) cfg->fall_speed = 0.10f;
                        if (cfg->fall_speed > 4.00f) cfg->fall_speed = 4.00f;
                        grid->fall_speed = cfg->fall_speed;
                        break;
                    case 1:
                        cfg->raindrop_length += dir * 0.10f;
                        if (cfg->raindrop_length < 0.30f) cfg->raindrop_length = 0.30f;
                        if (cfg->raindrop_length > 3.00f) cfg->raindrop_length = 3.00f;
                        grid->raindrop_length = cfg->raindrop_length;
                        break;
                    case 2:
                        cfg->glyph_cycle_speed += dir * 0.20f;
                        if (cfg->glyph_cycle_speed < 0.40f) cfg->glyph_cycle_speed = 0.40f;
                        if (cfg->glyph_cycle_speed > 5.00f) cfg->glyph_cycle_speed = 5.00f;
                        grid->glyph_cycle_speed = cfg->glyph_cycle_speed;
                        break;
                    case 3:
                        cfg->forward_speed += dir * 0.05f;
                        if (cfg->forward_speed < 0.05f) cfg->forward_speed = 0.05f;
                        if (cfg->forward_speed > 2.00f) cfg->forward_speed = 2.00f;
                        grid->forward_speed = cfg->forward_speed;
                        break;
                    case 4:
                        cfg->slant = (cfg->slant == 0.0f) ? 0.35f : (cfg->slant > 0.0f ? -0.25f : 0.0f);
                        grid->slant = cfg->slant;
                        break;
                }
            } else if (btn == SDL_CONTROLLER_BUTTON_A) {
                if (selected_item == 5) cfg->paused = !cfg->paused;
                else if (selected_item == 6) {
                    cfg->fullscreen = !cfg->fullscreen;
                    SDL_SetWindowFullscreen(window, cfg->fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                } else if (selected_item == 7) {
                    config_set_defaults(cfg);
                    sync_grid_with_config(grid, cfg);
                }
            }
            return true;
        }
    }

    /* Mouse click navigation inside modal */
    if (ev->type == SDL_MOUSEBUTTONDOWN && ev->button.button == SDL_BUTTON_LEFT) {
        int win_w = 0, win_h = 0;
        SDL_GetWindowSize(window, &win_w, &win_h);

        int panel_w = (win_w < 620) ? (win_w - 20) : (win_w < 800 ? 580 : 660);
        int panel_h = (win_h < 460) ? (win_h - 20) : (win_h < 600 ? 420 : 470);
        int panel_x = (win_w - panel_w) / 2;
        int panel_y = (win_h - panel_h) / 2;

        /* Close button [X] at top-right of panel */
        if (is_point_in_rect(ev->button.x, ev->button.y, panel_x + panel_w - 30, panel_y + 10, 20, 20)) {
            gui_close(cfg);
            return true;
        }

        /* Outside panel click closes */
        if (!is_point_in_rect(ev->button.x, ev->button.y, panel_x, panel_y, panel_w, panel_h)) {
            gui_close(cfg);
            return true;
        }

        /* Tab bar clicks */
        int tab_bar_y = panel_y + 36;
        int tab_w = (panel_w - 20) / TAB_COUNT;
        for (int t = 0; t < TAB_COUNT; t++) {
            if (is_point_in_rect(ev->button.x, ev->button.y, panel_x + 10 + t * tab_w, tab_bar_y, tab_w - 4, 24)) {
                current_tab = t;
                selected_item = 0;
                return true;
            }
        }

        int content_y = tab_bar_y + 35;

        if (current_tab == 0) {
            /* Versions grid click */
            int col_w = (panel_w - 40) / 2;
            int row_h = 28;
            for (int v = 0; v < VERSION_COUNT; v++) {
                int col = v / 7;
                int row = v % 7;
                int item_x = panel_x + 20 + col * col_w;
                int item_y = content_y + 8 + row * row_h;
                if (is_point_in_rect(ev->button.x, ev->button.y, item_x, item_y, col_w - 10, row_h - 4)) {
                    selected_item = v;
                    config_apply_version(cfg, (MatrixVersion)v);
                    sync_grid_with_config(grid, cfg);
                    return true;
                }
            }
        } else if (current_tab == 1) {
            /* Effects clicks */
            int item_h = 32;
            for (int i = 0; i < 6; i++) {
                int item_y = content_y + 10 + i * item_h;
                if (is_point_in_rect(ev->button.x, ev->button.y, panel_x + 20, item_y, panel_w - 40, item_h - 4)) {
                    selected_item = i;
                    switch (i) {
                        case 0: cfg->effect = (MatrixEffect)((cfg->effect + 1) % EFFECT_COUNT); break;
                        case 1: cfg->palette = (PaletteType)((cfg->palette + 1) % PALETTE_COUNT); break;
                        case 2: cfg->glow_effect = !cfg->glow_effect; break;
                        case 3: cfg->bonus_glyphs = !cfg->bonus_glyphs; grid->bonus_glyphs = cfg->bonus_glyphs; break;
                        case 4: cfg->render_mode = (cfg->render_mode == RENDER_MODE_COLOR) ? RENDER_MODE_PLAYDATE : RENDER_MODE_COLOR; break;
                        case 5: cfg->volumetric = !cfg->volumetric; grid->volumetric = cfg->volumetric; break;
                    }
                    return true;
                }
            }
        } else if (current_tab == 2) {
            /* Tuning clicks */
            int item_h = 32;
            for (int i = 0; i < 8; i++) {
                int item_y = content_y + 10 + i * item_h;
                if (is_point_in_rect(ev->button.x, ev->button.y, panel_x + 20, item_y, panel_w - 40, item_h - 4)) {
                    selected_item = i;
                    switch (i) {
                        case 0:
                            cfg->fall_speed = (cfg->fall_speed >= 3.0f) ? 0.5f : (cfg->fall_speed + 0.5f);
                            grid->fall_speed = cfg->fall_speed;
                            break;
                        case 1:
                            cfg->raindrop_length = (cfg->raindrop_length >= 2.5f) ? 0.5f : (cfg->raindrop_length + 0.4f);
                            grid->raindrop_length = cfg->raindrop_length;
                            break;
                        case 2:
                            cfg->glyph_cycle_speed = (cfg->glyph_cycle_speed >= 3.5f) ? 0.8f : (cfg->glyph_cycle_speed + 0.6f);
                            grid->glyph_cycle_speed = cfg->glyph_cycle_speed;
                            break;
                        case 3:
                            cfg->forward_speed = (cfg->forward_speed >= 1.5f) ? 0.1f : (cfg->forward_speed + 0.15f);
                            grid->forward_speed = cfg->forward_speed;
                            break;
                        case 4:
                            cfg->slant = (cfg->slant == 0.0f) ? 0.35f : (cfg->slant > 0.0f ? -0.25f : 0.0f);
                            grid->slant = cfg->slant;
                            break;
                        case 5:
                            cfg->paused = !cfg->paused;
                            break;
                        case 6:
                            cfg->fullscreen = !cfg->fullscreen;
                            SDL_SetWindowFullscreen(window, cfg->fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                            break;
                        case 7:
                            config_set_defaults(cfg);
                            sync_grid_with_config(grid, cfg);
                            break;
                    }
                    return true;
                }
            }
        }

        return true;
    }

    return true;
}

void gui_render(SDL_Renderer *renderer, const AppConfig *cfg, const MatrixGrid *grid) {
    if (!renderer || !cfg) return;
    (void)grid;

    int win_w = 0, win_h = 0;
    SDL_GetRendererOutputSize(renderer, &win_w, &win_h);

    int scale = (win_w >= 1000 && win_h >= 700) ? 2 : 1;
    (void)scale;

    SDL_Color c_green_bright = {0, 255, 65, 255};
    SDL_Color c_green_dim    = {0, 160, 40, 255};
    SDL_Color c_white        = {255, 255, 255, 255};
    SDL_Color c_gray         = {150, 160, 150, 255};
    SDL_Color c_gold         = {255, 215, 0, 255};
    SDL_Color c_cyan         = {0, 230, 255, 255};
    SDL_Color c_bg_dark      = {8, 14, 10, 240};
    SDL_Color c_bg_item      = {16, 26, 20, 240};
    SDL_Color c_bg_selected  = {0, 80, 30, 255};

    if (!cfg->settings_gui_open) {
        /* Subtle gear button in bottom-right corner */
        int badge_w = 140;
        int badge_h = 24;
        int badge_x = win_w - badge_w - 10;
        int badge_y = win_h - badge_h - 10;

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        fill_rect(renderer, badge_x, badge_y, badge_w, badge_h, (SDL_Color){10, 20, 12, 190});
        draw_rect(renderer, badge_x, badge_y, badge_w, badge_h, c_green_dim);
        draw_text(renderer, badge_x + 8, badge_y + 8, "[TAB: Settings]", c_green_bright, 1);
        return;
    }

    /* Modal dialog sizing */
    int panel_w = (win_w < 620) ? (win_w - 20) : (win_w < 800 ? 580 : 660);
    int panel_h = (win_h < 460) ? (win_h - 20) : (win_h < 600 ? 420 : 470);
    int panel_x = (win_w - panel_w) / 2;
    int panel_y = (win_h - panel_h) / 2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    /* Dim entire background */
    fill_rect(renderer, 0, 0, win_w, win_h, (SDL_Color){0, 0, 0, 160});

    /* Main settings window body */
    fill_rect(renderer, panel_x, panel_y, panel_w, panel_h, c_bg_dark);

    /* Outer border and inner frame */
    draw_rect(renderer, panel_x, panel_y, panel_w, panel_h, c_green_bright);
    draw_rect(renderer, panel_x + 2, panel_y + 2, panel_w - 4, panel_h - 4, c_green_dim);

    /* Title bar */
    draw_text(renderer, panel_x + 14, panel_y + 12, "THE MATRIX // SETTINGS & EFFECTS", c_white, 1);
    draw_text(renderer, panel_x + panel_w - 24, panel_y + 12, "[X]", c_cyan, 1);

    /* Tab header buttons */
    int tab_bar_y = panel_y + 34;
    int tab_w = (panel_w - 20) / TAB_COUNT;
    for (int t = 0; t < TAB_COUNT; t++) {
        int tx = panel_x + 10 + t * tab_w;
        bool is_active = (t == current_tab);

        if (is_active) {
            fill_rect(renderer, tx, tab_bar_y, tab_w - 4, 24, c_green_bright);
            draw_text(renderer, tx + 6, tab_bar_y + 8, tab_names[t], (SDL_Color){0, 0, 0, 255}, 1);
        } else {
            fill_rect(renderer, tx, tab_bar_y, tab_w - 4, 24, c_bg_item);
            draw_rect(renderer, tx, tab_bar_y, tab_w - 4, 24, c_green_dim);
            draw_text(renderer, tx + 6, tab_bar_y + 8, tab_names[t], c_green_dim, 1);
        }
    }

    int content_y = tab_bar_y + 35;

    /* TAB CONTENTS */
    if (current_tab == 0) {
        /* TAB 0: VERSIONS */
        draw_text(renderer, panel_x + 20, content_y - 8, "Select a canonical Matrix version (press ENTER or click):", c_gray, 1);

        int col_w = (panel_w - 40) / 2;
        int row_h = 28;

        for (int v = 0; v < VERSION_COUNT; v++) {
            int col = v / 7;
            int row = v % 7;
            int item_x = panel_x + 20 + col * col_w;
            int item_y = content_y + 8 + row * row_h;
            bool is_selected = (v == selected_item);
            bool is_active = (v == (int)cfg->version);

            if (is_selected) {
                fill_rect(renderer, item_x, item_y, col_w - 10, row_h - 4, c_bg_selected);
                draw_rect(renderer, item_x, item_y, col_w - 10, row_h - 4, c_green_bright);
            } else {
                fill_rect(renderer, item_x, item_y, col_w - 10, row_h - 4, c_bg_item);
                draw_rect(renderer, item_x, item_y, col_w - 10, row_h - 4, is_active ? c_green_dim : (SDL_Color){30, 45, 35, 255});
            }

            char label[64];
            snprintf(label, sizeof(label), "%s %s", is_active ? ">" : " ", config_version_name((MatrixVersion)v));
            draw_text(renderer, item_x + 8, item_y + 7, label, is_active ? c_gold : (is_selected ? c_white : c_green_bright), 1);
            if (is_active) {
                draw_text(renderer, item_x + col_w - 60, item_y + 7, "[ACTIVE]", c_gold, 1);
            }
        }

        /* Description footer */
        int desc_y = panel_y + panel_h - 55;
        fill_rect(renderer, panel_x + 16, desc_y, panel_w - 32, 42, (SDL_Color){12, 22, 16, 255});
        draw_rect(renderer, panel_x + 16, desc_y, panel_w - 32, 42, c_green_dim);
        draw_text(renderer, panel_x + 24, desc_y + 8, config_version_name((MatrixVersion)selected_item), c_cyan, 1);
        draw_text(renderer, panel_x + 24, desc_y + 24, config_version_desc((MatrixVersion)selected_item), c_gray, 1);

    } else if (current_tab == 1) {
        /* TAB 1: EFFECTS */
        draw_text(renderer, panel_x + 20, content_y - 8, "Configure coloration, post-processing & flag stripes:", c_gray, 1);

        int item_h = 32;
        for (int i = 0; i < 6; i++) {
            int item_y = content_y + 10 + i * item_h;
            bool is_sel = (i == selected_item);

            if (is_sel) {
                fill_rect(renderer, panel_x + 20, item_y, panel_w - 40, item_h - 5, c_bg_selected);
                draw_rect(renderer, panel_x + 20, item_y, panel_w - 40, item_h - 5, c_green_bright);
            } else {
                fill_rect(renderer, panel_x + 20, item_y, panel_w - 40, item_h - 5, c_bg_item);
                draw_rect(renderer, panel_x + 20, item_y, panel_w - 40, item_h - 5, c_green_dim);
            }

            char label[64];
            char value[64];
            switch (i) {
                case 0:
                    snprintf(label, sizeof(label), "Effect Style");
                    snprintf(value, sizeof(value), "< %s >", config_effect_name(cfg->effect));
                    break;
                case 1:
                    snprintf(label, sizeof(label), "Base Palette");
                    snprintf(value, sizeof(value), "< %s >", config_version_name((MatrixVersion)cfg->palette));
                    break;
                case 2:
                    snprintf(label, sizeof(label), "Cursor Glow Halo");
                    snprintf(value, sizeof(value), "[ %s ]", cfg->glow_effect ? "ON" : "OFF");
                    break;
                case 3:
                    snprintf(label, sizeof(label), "Playdate Bonus Glyphs");
                    snprintf(value, sizeof(value), "[ %s ]", cfg->bonus_glyphs ? "ON" : "OFF");
                    break;
                case 4:
                    snprintf(label, sizeof(label), "Renderer Engine");
                    snprintf(value, sizeof(value), "< %s >", (cfg->render_mode == RENDER_MODE_COLOR) ? "Color Phosphor" : "Playdate 1-Bit Dither");
                    break;
                case 5:
                    snprintf(label, sizeof(label), "3D Volumetric Mode");
                    snprintf(value, sizeof(value), "[ %s ]", cfg->volumetric ? "ENABLED" : "DISABLED");
                    break;
            }

            draw_text(renderer, panel_x + 32, item_y + 8, label, is_sel ? c_white : c_green_bright, 1);
            draw_text(renderer, panel_x + panel_w - 250, item_y + 8, value, is_sel ? c_gold : c_cyan, 1);
        }

        /* Color swatch preview */
        int swatch_y = content_y + 10 + 6 * item_h + 8;
        draw_text(renderer, panel_x + 24, swatch_y, "Live Preview Swatches:", c_gray, 1);
        int swatch_w = 40;
        int swatch_h = 18;

        if (cfg->effect == EFFECT_PRIDE) {
            static const SDL_Color swatches[6] = {
                {227, 2, 2, 255}, {255, 140, 0, 255}, {255, 237, 0, 255},
                {0, 128, 38, 255}, {0, 77, 255, 255}, {117, 7, 135, 255}
            };
            for (int s = 0; s < 6; s++) {
                fill_rect(renderer, panel_x + 24 + s * (swatch_w + 6), swatch_y + 16, swatch_w, swatch_h, swatches[s]);
            }
        } else if (cfg->effect == EFFECT_TRANS_PRIDE) {
            static const SDL_Color swatches[5] = {
                {92, 206, 250, 255}, {245, 169, 184, 255}, {255, 255, 255, 255},
                {245, 169, 184, 255}, {92, 206, 250, 255}
            };
            for (int s = 0; s < 5; s++) {
                fill_rect(renderer, panel_x + 24 + s * (swatch_w + 6), swatch_y + 16, swatch_w, swatch_h, swatches[s]);
            }
        } else {
            /* Standard palette swatch */
            fill_rect(renderer, panel_x + 24, swatch_y + 16, swatch_w * 4, swatch_h, (SDL_Color){0, 255, 65, 255});
            draw_text(renderer, panel_x + 24 + swatch_w * 4 + 10, swatch_y + 20, "[Tone-Mapped Phosphor Gradient]", c_green_bright, 1);
        }

    } else if (current_tab == 2) {
        /* TAB 2: TUNING */
        draw_text(renderer, panel_x + 20, content_y - 8, "Adjust physical rain parameters and window settings:", c_gray, 1);

        int item_h = 32;
        for (int i = 0; i < 8; i++) {
            int item_y = content_y + 8 + i * item_h;
            bool is_sel = (i == selected_item);

            if (is_sel) {
                fill_rect(renderer, panel_x + 20, item_y, panel_w - 40, item_h - 5, c_bg_selected);
                draw_rect(renderer, panel_x + 20, item_y, panel_w - 40, item_h - 5, c_green_bright);
            } else {
                fill_rect(renderer, panel_x + 20, item_y, panel_w - 40, item_h - 5, c_bg_item);
                draw_rect(renderer, panel_x + 20, item_y, panel_w - 40, item_h - 5, c_green_dim);
            }

            char label[64];
            char value[64];
            switch (i) {
                case 0:
                    snprintf(label, sizeof(label), "Rain Fall Speed");
                    snprintf(value, sizeof(value), "[-/+] %.2fx", cfg->fall_speed);
                    break;
                case 1:
                    snprintf(label, sizeof(label), "Raindrop Tail Length");
                    snprintf(value, sizeof(value), "[-/+] %.2f", cfg->raindrop_length);
                    break;
                case 2:
                    snprintf(label, sizeof(label), "Glyph Cycle Speed");
                    snprintf(value, sizeof(value), "[-/+] %.2fx", cfg->glyph_cycle_speed);
                    break;
                case 3:
                    snprintf(label, sizeof(label), "3D Approach Speed");
                    snprintf(value, sizeof(value), "[-/+] %.2fx", cfg->forward_speed);
                    break;
                case 4:
                    snprintf(label, sizeof(label), "Rain Slant Angle");
                    snprintf(value, sizeof(value), "< %s >", (cfg->slant > 0.0f) ? "+20 deg (Right)" : (cfg->slant < 0.0f ? "-15 deg (Left)" : "0 deg (Vertical)"));
                    break;
                case 5:
                    snprintf(label, sizeof(label), "Simulation State");
                    snprintf(value, sizeof(value), "[ %s ]", cfg->paused ? "PAUSED" : "RUNNING");
                    break;
                case 6:
                    snprintf(label, sizeof(label), "Display Mode");
                    snprintf(value, sizeof(value), "[ %s ]", cfg->fullscreen ? "FULLSCREEN" : "WINDOWED");
                    break;
                case 7:
                    snprintf(label, sizeof(label), "Reset Configuration");
                    snprintf(value, sizeof(value), "[ RESTORE DEFAULTS ]");
                    break;
            }

            draw_text(renderer, panel_x + 32, item_y + 8, label, is_sel ? c_white : c_green_bright, 1);
            draw_text(renderer, panel_x + panel_w - 240, item_y + 8, value, is_sel ? c_gold : c_cyan, 1);
        }

    } else if (current_tab == 3) {
        /* TAB 3: ABOUT */
        int ty = content_y + 6;
        draw_text(renderer, panel_x + 24, ty, "MATRIX CODE RAIN (SDL2 MULTI-PLATFORM)", c_gold, 1);
        ty += 16;
        draw_text(renderer, panel_x + 24, ty, "Based on Rezmason's digital rain simulator:", c_gray, 1);
        ty += 14;
        draw_text(renderer, panel_x + 24, ty, "https://github.com/a18project/matrix", c_cyan, 1);
        ty += 22;

        draw_text(renderer, panel_x + 24, ty, "CONTROLS GUIDE:", c_white, 1);
        ty += 16;
        draw_text(renderer, panel_x + 32, ty, "Desktop Keyboard & Mouse:", c_green_bright, 1);
        ty += 14;
        draw_text(renderer, panel_x + 40, ty, "- TAB / F2 / Esc : Toggle or close this Settings Menu", c_gray, 1);
        ty += 12;
        draw_text(renderer, panel_x + 40, ty, "- 3 / V          : Quick toggle 3D Volumetric flythrough", c_gray, 1);
        ty += 12;
        draw_text(renderer, panel_x + 40, ty, "- Arrows / Mouse: Navigate options, click to select", c_gray, 1);
        ty += 12;
        draw_text(renderer, panel_x + 40, ty, "- M : Toggle Full-Color vs Playdate 1-Bit mode", c_gray, 1);
        ty += 12;
        draw_text(renderer, panel_x + 40, ty, "- P / C : Quick cycle palettes", c_gray, 1);
        ty += 12;
        draw_text(renderer, panel_x + 40, ty, "- Space : Pause / Resume   |   F11 : Fullscreen", c_gray, 1);
        ty += 18;

        draw_text(renderer, panel_x + 32, ty, "Handheld Gamepad (Miyoo Flip):", c_green_bright, 1);
        ty += 14;
        draw_text(renderer, panel_x + 40, ty, "- SELECT / MENU  : Toggle this Settings Menu", c_gray, 1);
        ty += 12;
        draw_text(renderer, panel_x + 40, ty, "- D-PAD / L1 / R1: Navigate tabs and adjust values", c_gray, 1);
        ty += 12;
        draw_text(renderer, panel_x + 40, ty, "- BUTTON A / B   : Select option / Go back / Close", c_gray, 1);
        ty += 12;
        draw_text(renderer, panel_x + 40, ty, "- BUTTON START   : Pause or resume simulation", c_gray, 1);
    }

    /* Bottom helper bar */
    int foot_y = panel_y + panel_h - 18;
    draw_text(renderer, panel_x + 14, foot_y, "[TAB/L1/R1: Tab]  [ARROWS/D-PAD: Navigate]  [ENTER/A: Select]  [ESC/B: Close]", c_green_dim, 1);
}
