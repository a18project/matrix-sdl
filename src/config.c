#include "config.h"
#include <string.h>

void config_set_defaults(AppConfig *cfg) {
    if (!cfg) return;
    cfg->window_width = 1280;
    cfg->window_height = 720;
    cfg->fullscreen = false;
    cfg->vsync = true;
    cfg->fall_speed = 1.0f;
    cfg->glyph_cycle_speed = 1.8f;
    cfg->raindrop_length = 1.15f;
    cfg->slant = 0.0f;
    cfg->volumetric = false;
    cfg->forward_speed = 0.25f;
    cfg->paused = false;
    cfg->render_mode = RENDER_MODE_COLOR;
    cfg->version = VERSION_CLASSIC;
    cfg->effect = EFFECT_PALETTE;
    cfg->palette = PALETTE_CLASSIC;
    cfg->glow_effect = true;
    cfg->bonus_glyphs = false;
    cfg->settings_gui_open = false;
    cfg->show_hud = false;
    cfg->assets_path = NULL;
    cfg->test_frames = 0;
}

void config_apply_version(AppConfig *cfg, MatrixVersion ver) {
    if (!cfg || ver < 0 || ver >= VERSION_COUNT) return;
    cfg->version = ver;
    cfg->volumetric = false;

    switch (ver) {
        case VERSION_CLASSIC:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_CLASSIC;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 1.0f;
            cfg->raindrop_length = 1.15f;
            cfg->glyph_cycle_speed = 1.8f;
            cfg->slant = 0.0f;
            break;

        case VERSION_RESURRECTIONS:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_RESURRECTIONS;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 1.1f;
            cfg->raindrop_length = 1.0f;
            cfg->glyph_cycle_speed = 1.8f;
            cfg->slant = 0.0f;
            break;

        case VERSION_OPERATOR:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_OPERATOR;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 1.6f;
            cfg->raindrop_length = 1.8f;
            cfg->glyph_cycle_speed = 1.0f;
            cfg->slant = 0.0f;
            break;

        case VERSION_NIGHTMARE:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_NIGHTMARE;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 2.2f;
            cfg->raindrop_length = 0.6f;
            cfg->glyph_cycle_speed = 3.5f;
            cfg->slant = 0.35f;
            break;

        case VERSION_PARADISE:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_PARADISE;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 0.3f;
            cfg->raindrop_length = 0.5f;
            cfg->glyph_cycle_speed = 0.6f;
            cfg->slant = 0.0f;
            break;

        case VERSION_PALIMPSEST:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_PALIMPSEST;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 1.2f;
            cfg->raindrop_length = 1.3f;
            cfg->glyph_cycle_speed = 1.2f;
            cfg->slant = -0.25f;
            break;

        case VERSION_TWILIGHT:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_TWILIGHT;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 0.6f;
            cfg->raindrop_length = 1.0f;
            cfg->glyph_cycle_speed = 1.5f;
            cfg->slant = 0.0f;
            break;

        case VERSION_MORPHEUS:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_MORPHEUS;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 0.9f;
            cfg->raindrop_length = 0.8f;
            cfg->glyph_cycle_speed = 1.2f;
            cfg->slant = 0.0f;
            break;

        case VERSION_TRINITY:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_TRINITY;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 1.0f;
            cfg->raindrop_length = 0.7f;
            cfg->glyph_cycle_speed = 1.0f;
            cfg->slant = 0.0f;
            break;

        case VERSION_BUGS:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_BUGS;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 1.1f;
            cfg->raindrop_length = 0.7f;
            cfg->glyph_cycle_speed = 1.0f;
            cfg->slant = 0.0f;
            break;

        case VERSION_MEGACITY:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_CLASSIC;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 0.5f;
            cfg->raindrop_length = 1.15f;
            cfg->glyph_cycle_speed = 1.0f;
            cfg->slant = 0.0f;
            break;

        case VERSION_PLAYDATE:
            cfg->render_mode = RENDER_MODE_PLAYDATE;
            cfg->effect = EFFECT_PALETTE;
            cfg->fall_speed = 1.0f;
            cfg->raindrop_length = 1.15f;
            cfg->glyph_cycle_speed = 1.8f;
            cfg->slant = 0.0f;
            break;

        case VERSION_3D:
            cfg->render_mode = RENDER_MODE_COLOR;
            cfg->palette = PALETTE_CLASSIC;
            cfg->effect = EFFECT_PALETTE;
            cfg->volumetric = true;
            cfg->fall_speed = 0.5f;
            cfg->forward_speed = 0.25f;
            cfg->raindrop_length = 0.8f;
            cfg->glyph_cycle_speed = 1.2f;
            cfg->slant = 0.0f;
            break;

        default:
            break;
    }
}

const char *config_version_name(MatrixVersion ver) {
    switch (ver) {
        case VERSION_CLASSIC:       return "Classic (1999)";
        case VERSION_RESURRECTIONS: return "Resurrections (2021)";
        case VERSION_OPERATOR:      return "Operator Terminal";
        case VERSION_NIGHTMARE:     return "Nightmare (Reloaded)";
        case VERSION_PARADISE:      return "Paradise (Gnostic)";
        case VERSION_PALIMPSEST:    return "Palimpsest (Furious Angels)";
        case VERSION_TWILIGHT:      return "Twilight (Cyberpunk)";
        case VERSION_MORPHEUS:      return "Morpheus (Zion)";
        case VERSION_TRINITY:       return "Trinity (Awakened)";
        case VERSION_BUGS:          return "Bugs (Blue Pill)";
        case VERSION_MEGACITY:      return "Megacity (Revolutions)";
        case VERSION_PLAYDATE:      return "Playdate (1-Bit Retro)";
        case VERSION_3D:            return "3D Volumetric";
        default:                    return "Unknown";
    }
}

const char *config_version_desc(MatrixVersion ver) {
    switch (ver) {
        case VERSION_CLASSIC:
            return "The iconic phosphor green digital rain as seen in the trilogy.";
        case VERSION_RESURRECTIONS:
            return "Modern Matrix Resurrections code with crisp mint accents.";
        case VERSION_OPERATOR:
            return "Flatter, crowded 1999 operator monitor with rapid streams.";
        case VERSION_NIGHTMARE:
            return "Merovingian's gothic vampire code: foreboding crimson & amber.";
        case VERSION_PARADISE:
            return "The first idyllic predecessor Matrix: hypnotic golden amber.";
        case VERSION_PALIMPSEST:
            return "Rob Dougan Furious Angels inspired: teal & gold contrast.";
        case VERSION_TWILIGHT:
            return "Vibrant futuristic palette with cyan, neon magenta & indigo.";
        case VERSION_MORPHEUS:
            return "Deep royal purple and crimson tones honoring Morpheus.";
        case VERSION_TRINITY:
            return "Emerald matrix streams illuminated by radiant gold glints.";
        case VERSION_BUGS:
            return "Electric cerulean & cyan code styling inspired by Bugs.";
        case VERSION_MEGACITY:
            return "Revolutions opening titles variation with slower descent.";
        case VERSION_PLAYDATE:
            return "Authentic 32-step dithered black-and-white handheld render.";
        case VERSION_3D:
            return "Volumetric 3D perspective flythrough with infinite depth.";
        default:
            return "";
    }
}

const char *config_effect_name(MatrixEffect eff) {
    switch (eff) {
        case EFFECT_PALETTE:     return "Standard Palette";
        case EFFECT_PRIDE:       return "Rainbow Pride Stripes";
        case EFFECT_TRANS_PRIDE: return "Trans Pride Stripes";
        case EFFECT_STRIPES:     return "Cyberpunk Dual Stripes";
        default:                 return "Unknown";
    }
}
