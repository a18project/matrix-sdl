#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

#include "config.h"
#include "matrix.h"
#include "renderer.h"
#include "gui.h"

static void print_help(const char *prog_name) {
    printf("Matrix Code Rain (SDL2 Multi-Platform Port)\n");
    printf("Based on https://github.com/a18project/matrix by Rezmason\n\n");
    printf("Usage:\n");
    printf("  %s [options]\n\n", prog_name);
    printf("Options:\n");
    printf("  -v, --version <name>           Matrix version: classic, resurrections, operator,\n");
    printf("                                 nightmare, paradise, palimpsest, twilight,\n");
    printf("                                 morpheus, trinity, bugs, megacity, playdate, 3d\n");
    printf("  -e, --effect <name>            Effect: palette, pride, trans, stripes\n");
    printf("  -m, --mode <color|playdate>    Rendering mode (default: color)\n");
    printf("  -p, --palette <name>           Color palette: classic, resurrections, operator,\n");
    printf("                                 nightmare, paradise, palimpsest, twilight,\n");
    printf("                                 morpheus, trinity, bugs, terminal, amber\n");
    printf("  -s, --speed <float>            Fall speed multiplier (default: 1.0)\n");
    printf("  -l, --length <float>           Raindrop tail length (default: 1.15)\n");
    printf("  --slant <float>                Rain slant angle (-0.5 to 0.5)\n");
    printf("  -3, --3d, --volumetric         Volumetric 3D flythrough perspective\n");
    printf("  --forward-speed <float>        3D camera approach speed (default: 0.25)\n");
    printf("  -w, --width <pixels>           Initial window width (default: 1280)\n");
    printf("  -h, --height <pixels>          Initial window height (default: 720)\n");
    printf("  -f, --fullscreen               Start in fullscreen mode\n");
    printf("  -a, --assets <path>            Path to assets directory\n");
    printf("  --frames <count>               Run for N frames and exit (useful for automated testing)\n");
    printf("  --no-glow                      Disable glow halo around cursors\n");
    printf("  --help                         Show this help message\n\n");
    printf("Interactive Controls:\n");
    printf("  [Tab] / [F2]  Toggle in-game Settings & Effects GUI Menu\n");
    printf("  [3] / [V]     Toggle 3D Volumetric Flythrough Mode\n");
    printf("  [M]           Toggle Mode (Color / Playdate 1-Bit Dither)\n");
    printf("  [P] / [C]     Cycle Color Palettes\n");
    printf("  [Space]       Pause / Resume\n");
    printf("  [Up/Down]     Increase / Decrease fall speed\n");
    printf("  [ [ / ] ]     Decrease / Increase raindrop length\n");
    printf("  [G]           Toggle Glow Effect\n");
    printf("  [B]           Toggle Bonus Glyphs (Playdate characters)\n");
    printf("  [F] / [F11]   Toggle Fullscreen\n");
    printf("  [Esc] / [Q]   Quit\n\n");
}

static void parse_args(int argc, char *argv[], AppConfig *cfg) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help(argv[0]);
            exit(0);
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fullscreen") == 0) {
            cfg->fullscreen = true;
        } else if ((strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "resurrections") == 0) config_apply_version(cfg, VERSION_RESURRECTIONS);
            else if (strcmp(argv[i], "operator") == 0 || strcmp(argv[i], "1999") == 0) config_apply_version(cfg, VERSION_OPERATOR);
            else if (strcmp(argv[i], "nightmare") == 0) config_apply_version(cfg, VERSION_NIGHTMARE);
            else if (strcmp(argv[i], "paradise") == 0) config_apply_version(cfg, VERSION_PARADISE);
            else if (strcmp(argv[i], "palimpsest") == 0) config_apply_version(cfg, VERSION_PALIMPSEST);
            else if (strcmp(argv[i], "twilight") == 0) config_apply_version(cfg, VERSION_TWILIGHT);
            else if (strcmp(argv[i], "morpheus") == 0) config_apply_version(cfg, VERSION_MORPHEUS);
            else if (strcmp(argv[i], "trinity") == 0) config_apply_version(cfg, VERSION_TRINITY);
            else if (strcmp(argv[i], "bugs") == 0) config_apply_version(cfg, VERSION_BUGS);
            else if (strcmp(argv[i], "megacity") == 0) config_apply_version(cfg, VERSION_MEGACITY);
            else if (strcmp(argv[i], "playdate") == 0) config_apply_version(cfg, VERSION_PLAYDATE);
            else if (strcmp(argv[i], "3d") == 0 || strcmp(argv[i], "volumetric") == 0) config_apply_version(cfg, VERSION_3D);
            else config_apply_version(cfg, VERSION_CLASSIC);
        } else if ((strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--effect") == 0) && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "pride") == 0) cfg->effect = EFFECT_PRIDE;
            else if (strcmp(argv[i], "trans") == 0) cfg->effect = EFFECT_TRANS_PRIDE;
            else if (strcmp(argv[i], "stripes") == 0) cfg->effect = EFFECT_STRIPES;
            else cfg->effect = EFFECT_PALETTE;
        } else if ((strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--mode") == 0) && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "playdate") == 0 || strcmp(argv[i], "1bit") == 0) {
                cfg->render_mode = RENDER_MODE_PLAYDATE;
            } else {
                cfg->render_mode = RENDER_MODE_COLOR;
            }
        } else if ((strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--palette") == 0) && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "resurrections") == 0) cfg->palette = PALETTE_RESURRECTIONS;
            else if (strcmp(argv[i], "operator") == 0)  cfg->palette = PALETTE_OPERATOR;
            else if (strcmp(argv[i], "nightmare") == 0) cfg->palette = PALETTE_NIGHTMARE;
            else if (strcmp(argv[i], "paradise") == 0)  cfg->palette = PALETTE_PARADISE;
            else if (strcmp(argv[i], "palimpsest") == 0) cfg->palette = PALETTE_PALIMPSEST;
            else if (strcmp(argv[i], "twilight") == 0)  cfg->palette = PALETTE_TWILIGHT;
            else if (strcmp(argv[i], "morpheus") == 0)  cfg->palette = PALETTE_MORPHEUS;
            else if (strcmp(argv[i], "trinity") == 0)   cfg->palette = PALETTE_TRINITY;
            else if (strcmp(argv[i], "bugs") == 0)      cfg->palette = PALETTE_BUGS;
            else if (strcmp(argv[i], "terminal") == 0)  cfg->palette = PALETTE_TERMINAL_WHITE;
            else if (strcmp(argv[i], "amber") == 0)     cfg->palette = PALETTE_AMBER_CRT;
            else cfg->palette = PALETTE_CLASSIC;
        } else if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--speed") == 0) && i + 1 < argc) {
            cfg->fall_speed = (float)atof(argv[++i]);
        } else if ((strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--length") == 0) && i + 1 < argc) {
            cfg->raindrop_length = (float)atof(argv[++i]);
        } else if (strcmp(argv[i], "--slant") == 0 && i + 1 < argc) {
            cfg->slant = (float)atof(argv[++i]);
        } else if (strcmp(argv[i], "-3") == 0 || strcmp(argv[i], "--3d") == 0 || strcmp(argv[i], "--volumetric") == 0) {
            cfg->volumetric = true;
        } else if (strcmp(argv[i], "--forward-speed") == 0 && i + 1 < argc) {
            cfg->forward_speed = (float)atof(argv[++i]);
        } else if ((strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--width") == 0) && i + 1 < argc) {
            cfg->window_width = atoi(argv[++i]);
        } else if ((strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--height") == 0) && i + 1 < argc) {
            cfg->window_height = atoi(argv[++i]);
        } else if ((strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--assets") == 0) && i + 1 < argc) {
            cfg->assets_path = argv[++i];
        } else if (strcmp(argv[i], "--frames") == 0 && i + 1 < argc) {
            cfg->test_frames = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--no-glow") == 0) {
            cfg->glow_effect = false;
        }
    }
}

static void update_window_title(SDL_Window *win, const MatrixRenderer *r, const AppConfig *cfg, float fps) {
    char title[256];
    const char *mode_str = (cfg->render_mode == RENDER_MODE_COLOR) ? "Color" : "1-Bit";
    const ColorPalette *pal = matrix_renderer_get_palette(r, cfg->palette);
    const char *pal_name = pal ? pal->name : "Custom";
    const char *ver_name = config_version_name(cfg->version);
    const char *eff_name = (cfg->effect != EFFECT_PALETTE) ? config_effect_name(cfg->effect) : pal_name;

    snprintf(title, sizeof(title), "Matrix Rain [SDL2%s] | %s | %s | %s | %.1fx | FPS: %.0f%s",
             cfg->volumetric ? " 3D" : "", ver_name, eff_name, mode_str, cfg->fall_speed, fps, cfg->paused ? " (PAUSED)" : "");

    SDL_SetWindowTitle(win, title);
}

int main(int argc, char *argv[]) {
    AppConfig cfg;
    config_set_defaults(&cfg);
    parse_args(argc, argv, &cfg);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK) != 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    /* Open connected gamepads / joysticks (for handheld consoles like Miyoo Flip) */
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            SDL_GameControllerOpen(i);
        } else {
            SDL_JoystickOpen(i);
        }
    }

    Uint32 window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    if (cfg.fullscreen) {
        window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Matrix Rain (SDL2)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        cfg.window_width,
        cfg.window_height,
        window_flags
    );

    if (!window) {
        fprintf(stderr, "Failed to create SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    if (cfg.vsync) {
        render_flags |= SDL_RENDERER_PRESENTVSYNC;
    }

    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(window, -1, render_flags);
    if (!sdl_renderer) {
        /* Fall back to software renderer if hardware accelerated fails */
        sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        if (!sdl_renderer) {
            fprintf(stderr, "Failed to create SDL renderer: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    }

    MatrixRenderer *renderer = matrix_renderer_create(window, sdl_renderer, cfg.assets_path);
    if (!renderer) {
        fprintf(stderr, "Failed to initialize Matrix renderer.\n");
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int win_w = 0, win_h = 0;
    SDL_GetRendererOutputSize(sdl_renderer, &win_w, &win_h);

    int num_cols = win_w / GLYPH_WIDTH;
    int num_rows = win_h / GLYPH_HEIGHT;
    if (num_cols < 1) num_cols = 1;
    if (num_rows < 1) num_rows = 1;

    MatrixGrid *grid = matrix_grid_create(num_cols, num_rows, &cfg);
    if (!grid) {
        fprintf(stderr, "Failed to create Matrix simulation grid.\n");
        matrix_renderer_destroy(renderer);
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Uint64 perf_freq = SDL_GetPerformanceFrequency();
    Uint64 last_time = SDL_GetPerformanceCounter();
    float fps_timer = 0.0f;
    int frame_count = 0;
    float current_fps = 60.0f;

    gui_init();

    bool running = true;
    SDL_Event ev;
    static bool joy_select_down = false;
    static bool joy_start_down = false;

    while (running) {
        if (cfg.should_quit) {
            running = false;
            break;
        }

        while (SDL_PollEvent(&ev)) {
            if (gui_handle_event(&ev, &cfg, grid, window)) {
                if (cfg.should_quit) {
                    running = false;
                    break;
                }
                continue;
            }

            switch (ev.type) {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    switch (ev.key.keysym.sym) {
                        case SDLK_ESCAPE:
                        case SDLK_q:
                            running = false;
                            break;

                        case SDLK_SPACE:
                            cfg.paused = !cfg.paused;
                            break;

                        case SDLK_m:
                            /* Toggle between Full Color and 1-Bit Playdate Mode */
                            cfg.render_mode = (cfg.render_mode == RENDER_MODE_COLOR) ? RENDER_MODE_PLAYDATE : RENDER_MODE_COLOR;
                            break;

                        case SDLK_3:
                        case SDLK_v:
                            /* Toggle 3D Volumetric Mode */
                            cfg.volumetric = !cfg.volumetric;
                            grid->volumetric = cfg.volumetric;
                            break;

                        case SDLK_p:
                        case SDLK_c:
                            /* Cycle Color Palettes */
                            cfg.palette = (cfg.palette + 1) % PALETTE_COUNT;
                            break;

                        case SDLK_UP:
                            cfg.fall_speed += 0.15f;
                            if (cfg.fall_speed > 5.0f) cfg.fall_speed = 5.0f;
                            grid->fall_speed = cfg.fall_speed;
                            break;

                        case SDLK_DOWN:
                            cfg.fall_speed -= 0.15f;
                            if (cfg.fall_speed < 0.10f) cfg.fall_speed = 0.10f;
                            grid->fall_speed = cfg.fall_speed;
                            break;

                        case SDLK_RIGHTBRACKET:
                            cfg.raindrop_length += 0.10f;
                            if (cfg.raindrop_length > 4.0f) cfg.raindrop_length = 4.0f;
                            grid->raindrop_length = cfg.raindrop_length;
                            break;

                        case SDLK_LEFTBRACKET:
                            cfg.raindrop_length -= 0.10f;
                            if (cfg.raindrop_length < 0.30f) cfg.raindrop_length = 0.30f;
                            grid->raindrop_length = cfg.raindrop_length;
                            break;

                        case SDLK_g:
                            cfg.glow_effect = !cfg.glow_effect;
                            break;

                        case SDLK_b:
                            grid->bonus_glyphs = !grid->bonus_glyphs;
                            break;

                        case SDLK_f:
                        case SDLK_F11: {
                            cfg.fullscreen = !cfg.fullscreen;
                            SDL_SetWindowFullscreen(window, cfg.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                            break;
                        }

                        case SDLK_F1:
                            print_help(argv[0]);
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_CONTROLLERDEVICEADDED:
                    if (SDL_IsGameController(ev.cdevice.which)) {
                        SDL_GameControllerOpen(ev.cdevice.which);
                    }
                    break;

                case SDL_JOYDEVICEADDED:
                    if (!SDL_IsGameController(ev.jdevice.which)) {
                        SDL_JoystickOpen(ev.jdevice.which);
                    }
                    break;

                case SDL_CONTROLLERBUTTONDOWN:
                    if (ev.cbutton.button == SDL_CONTROLLER_BUTTON_BACK) joy_select_down = true;
                    if (ev.cbutton.button == SDL_CONTROLLER_BUTTON_START) joy_start_down = true;

                    /* Universal handheld exit combo: SELECT + START */
                    if (joy_select_down && joy_start_down) {
                        running = false;
                        break;
                    }

                    switch (ev.cbutton.button) {
                        case SDL_CONTROLLER_BUTTON_DPAD_UP:
                            cfg.fall_speed += 0.15f;
                            if (cfg.fall_speed > 5.0f) cfg.fall_speed = 5.0f;
                            grid->fall_speed = cfg.fall_speed;
                            break;
                        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                            cfg.fall_speed -= 0.15f;
                            if (cfg.fall_speed < 0.10f) cfg.fall_speed = 0.10f;
                            grid->fall_speed = cfg.fall_speed;
                            break;
                        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                            cfg.raindrop_length -= 0.10f;
                            if (cfg.raindrop_length < 0.30f) cfg.raindrop_length = 0.30f;
                            grid->raindrop_length = cfg.raindrop_length;
                            break;
                        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                            cfg.raindrop_length += 0.10f;
                            if (cfg.raindrop_length > 4.0f) cfg.raindrop_length = 4.0f;
                            grid->raindrop_length = cfg.raindrop_length;
                            break;
                        case SDL_CONTROLLER_BUTTON_A:
                            /* Toggle between Full Color and 1-Bit Playdate Mode */
                            cfg.render_mode = (cfg.render_mode == RENDER_MODE_COLOR) ? RENDER_MODE_PLAYDATE : RENDER_MODE_COLOR;
                            break;
                        case SDL_CONTROLLER_BUTTON_B:
                        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                            /* Next Palette */
                            cfg.palette = (cfg.palette + 1) % PALETTE_COUNT;
                            break;
                        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                            /* Previous Palette */
                            cfg.palette = (cfg.palette + PALETTE_COUNT - 1) % PALETTE_COUNT;
                            break;
                        case SDL_CONTROLLER_BUTTON_X:
                            cfg.glow_effect = !cfg.glow_effect;
                            break;
                        case SDL_CONTROLLER_BUTTON_Y:
                            grid->bonus_glyphs = !grid->bonus_glyphs;
                            break;
                        case SDL_CONTROLLER_BUTTON_START:
                            cfg.paused = !cfg.paused;
                            break;
                        case SDL_CONTROLLER_BUTTON_BACK:
                        case SDL_CONTROLLER_BUTTON_GUIDE:
                            running = false;
                            break;
                        default:
                            break;
                    }
                    break;

                case SDL_CONTROLLERBUTTONUP:
                    if (ev.cbutton.button == SDL_CONTROLLER_BUTTON_BACK) joy_select_down = false;
                    if (ev.cbutton.button == SDL_CONTROLLER_BUTTON_START) joy_start_down = false;
                    break;

                case SDL_JOYBUTTONDOWN:
                    if (ev.jbutton.button == 8) joy_select_down = true;
                    if (ev.jbutton.button == 9) joy_start_down = true;
                    if (joy_select_down && joy_start_down) {
                        running = false;
                        break;
                    }
                    if (ev.jbutton.button == 8 || ev.jbutton.button == 10 || ev.jbutton.button == 11) {
                        gui_toggle(&cfg);
                    }
                    break;

                case SDL_JOYBUTTONUP:
                    if (ev.jbutton.button == 8) joy_select_down = false;
                    if (ev.jbutton.button == 9) joy_start_down = false;
                    break;

                case SDL_JOYHATMOTION:
                    if (ev.jhat.value & SDL_HAT_UP) {
                        cfg.fall_speed += 0.15f;
                        if (cfg.fall_speed > 5.0f) cfg.fall_speed = 5.0f;
                        grid->fall_speed = cfg.fall_speed;
                    } else if (ev.jhat.value & SDL_HAT_DOWN) {
                        cfg.fall_speed -= 0.15f;
                        if (cfg.fall_speed < 0.10f) cfg.fall_speed = 0.10f;
                        grid->fall_speed = cfg.fall_speed;
                    }
                    if (ev.jhat.value & SDL_HAT_LEFT) {
                        cfg.raindrop_length -= 0.10f;
                        if (cfg.raindrop_length < 0.30f) cfg.raindrop_length = 0.30f;
                        grid->raindrop_length = cfg.raindrop_length;
                    } else if (ev.jhat.value & SDL_HAT_RIGHT) {
                        cfg.raindrop_length += 0.10f;
                        if (cfg.raindrop_length > 4.0f) cfg.raindrop_length = 4.0f;
                        grid->raindrop_length = cfg.raindrop_length;
                    }
                    break;

                case SDL_WINDOWEVENT:
                    if (ev.window.event == SDL_WINDOWEVENT_RESIZED ||
                        ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        int nw = 0, nh = 0;
                        SDL_GetRendererOutputSize(sdl_renderer, &nw, &nh);
                        int nc = nw / GLYPH_WIDTH;
                        int nr = nh / GLYPH_HEIGHT;
                        if (nc < 1) nc = 1;
                        if (nr < 1) nr = 1;
                        matrix_grid_resize(grid, nc, nr);
                    }
                    break;

                default:
                    break;
            }
        }

        Uint64 now = SDL_GetPerformanceCounter();
        float dt = (float)(now - last_time) / (float)perf_freq;
        last_time = now;

        /* Prevent frame delta spikes */
        if (dt > 0.1f) dt = 0.1f;

        /* Performance / FPS tracker */
        fps_timer += dt;
        frame_count++;
        if (fps_timer >= 1.0f) {
            current_fps = (float)frame_count / fps_timer;
            frame_count = 0;
            fps_timer = 0.0f;
            update_window_title(window, renderer, &cfg, current_fps);
        }

        /* Update simulation */
        if (!cfg.paused) {
            matrix_grid_update(grid, dt);
        }

        /* Render frame */
        matrix_renderer_render(renderer, grid, &cfg);
        gui_render(sdl_renderer, &cfg, grid);
        SDL_RenderPresent(sdl_renderer);

        if (cfg.test_frames > 0) {
            cfg.test_frames--;
            if (cfg.test_frames == 0) {
                running = false;
            }
        }
    }

    matrix_grid_destroy(grid);
    matrix_renderer_destroy(renderer);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
