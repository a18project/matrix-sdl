#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "matrix.h"

#define SQRT_2 1.4142135623730951f
#define SQRT_5 2.23606797749979f

static float randf(void) {
    return (float)rand() / (float)RAND_MAX;
}

static inline float wobble(float x) {
    return x + 0.3f * sinf(SQRT_2 * x) + 0.2f * sinf(SQRT_5 * x);
}

static inline float calculate_brightness(float sim_time, float col_offset, float col_speed, int x, int y, float raindrop_len, float fall_speed, float slant) {
    float col_time = col_offset + sim_time * fall_speed * col_speed;
    float rain_time = (col_time - (float)y * 0.05f + (float)x * slant * 0.04f) / raindrop_len;
    float w = wobble(rain_time);
    float fract = w - floorf(w);
    return 1.0f - fract;
}

MatrixGrid *matrix_grid_create(int num_columns, int num_rows, const AppConfig *cfg) {
    if (num_columns <= 0 || num_rows <= 0) return NULL;

    MatrixGrid *grid = malloc(sizeof(MatrixGrid));
    if (!grid) return NULL;

    grid->num_columns = num_columns;
    grid->num_rows = num_rows;
    grid->num_cells = num_columns * num_rows;
    grid->sim_time = 0.0f;
    grid->fall_speed = cfg->fall_speed;
    grid->glyph_cycle_speed = cfg->glyph_cycle_speed;
    grid->raindrop_length = cfg->raindrop_length;
    grid->slant = cfg->slant;
    grid->forward_speed = cfg->forward_speed;
    grid->volumetric = cfg->volumetric;
    grid->bonus_glyphs = cfg->bonus_glyphs;

    grid->column_time_offsets = malloc(sizeof(float) * num_columns);
    grid->column_speed_offsets = malloc(sizeof(float) * num_columns);
    grid->column_depths = malloc(sizeof(float) * num_columns);
    grid->column_x_norm = malloc(sizeof(float) * num_columns);
    grid->column_y_offset = malloc(sizeof(float) * num_columns);
    grid->cells = malloc(sizeof(MatrixCell) * grid->num_cells);

    if (!grid->column_time_offsets || !grid->column_speed_offsets ||
        !grid->column_depths || !grid->column_x_norm || !grid->column_y_offset ||
        !grid->cells) {
        matrix_grid_destroy(grid);
        return NULL;
    }

    /* Seed RNG with current time */
    srand((unsigned int)time(NULL));

    for (int x = 0; x < num_columns; x++) {
        grid->column_time_offsets[x] = randf() * 1000.0f;
        grid->column_speed_offsets[x] = randf() * 0.6f + 0.7f;
        grid->column_depths[x] = randf();
        float t = (num_columns > 1) ? ((float)x / (float)(num_columns - 1)) : 0.5f;
        grid->column_x_norm[x] = t * 2.4f - 1.2f + (randf() - 0.5f) * 0.05f;
        grid->column_y_offset[x] = (randf() - 0.5f) * 0.35f;
    }

    int idx = 0;
    for (int y = 0; y < num_rows; y++) {
        for (int x = 0; x < num_columns; x++) {
            MatrixCell *cell = &grid->cells[idx++];
            cell->x = x;
            cell->y = y;
            cell->glyph_index = rand() % NUM_STANDARD_GLYPHS;
            cell->glyph_cycle = randf();
            cell->brightness = 0.0f;
            cell->is_cursor = false;
            cell->fade_index = 0;
            cell->column_time_offset = grid->column_time_offsets[x];
            cell->column_speed_offset = grid->column_speed_offsets[x];
        }
    }

    return grid;
}

void matrix_grid_destroy(MatrixGrid *grid) {
    if (!grid) return;
    free(grid->column_time_offsets);
    free(grid->column_speed_offsets);
    free(grid->column_depths);
    free(grid->column_x_norm);
    free(grid->column_y_offset);
    free(grid->cells);
    free(grid);
}

void matrix_grid_resize(MatrixGrid *grid, int new_columns, int new_rows) {
    if (!grid || new_columns <= 0 || new_rows <= 0) return;
    if (grid->num_columns == new_columns && grid->num_rows == new_rows) return;

    float *new_time_offsets = malloc(sizeof(float) * new_columns);
    float *new_speed_offsets = malloc(sizeof(float) * new_columns);
    float *new_depths = malloc(sizeof(float) * new_columns);
    float *new_x_norm = malloc(sizeof(float) * new_columns);
    float *new_y_offset = malloc(sizeof(float) * new_columns);
    int new_cells_count = new_columns * new_rows;
    MatrixCell *new_cells = malloc(sizeof(MatrixCell) * new_cells_count);

    if (!new_time_offsets || !new_speed_offsets || !new_depths ||
        !new_x_norm || !new_y_offset || !new_cells) {
        free(new_time_offsets);
        free(new_speed_offsets);
        free(new_depths);
        free(new_x_norm);
        free(new_y_offset);
        free(new_cells);
        return;
    }

    for (int x = 0; x < new_columns; x++) {
        if (x < grid->num_columns) {
            new_time_offsets[x] = grid->column_time_offsets[x];
            new_speed_offsets[x] = grid->column_speed_offsets[x];
            new_depths[x] = grid->column_depths[x];
            new_x_norm[x] = grid->column_x_norm[x];
            new_y_offset[x] = grid->column_y_offset[x];
        } else {
            new_time_offsets[x] = randf() * 1000.0f;
            new_speed_offsets[x] = randf() * 0.6f + 0.7f;
            new_depths[x] = randf();
            float t = (new_columns > 1) ? ((float)x / (float)(new_columns - 1)) : 0.5f;
            new_x_norm[x] = t * 2.4f - 1.2f + (randf() - 0.5f) * 0.05f;
            new_y_offset[x] = (randf() - 0.5f) * 0.35f;
        }
    }

    int idx = 0;
    for (int y = 0; y < new_rows; y++) {
        for (int x = 0; x < new_columns; x++) {
            MatrixCell *cell = &new_cells[idx++];
            cell->x = x;
            cell->y = y;
            cell->column_time_offset = new_time_offsets[x];
            cell->column_speed_offset = new_speed_offsets[x];

            /* Preserve existing cell properties if within previous bounds */
            if (x < grid->num_columns && y < grid->num_rows) {
                int old_idx = y * grid->num_columns + x;
                cell->glyph_index = grid->cells[old_idx].glyph_index;
                cell->glyph_cycle = grid->cells[old_idx].glyph_cycle;
                cell->brightness = grid->cells[old_idx].brightness;
                cell->is_cursor = grid->cells[old_idx].is_cursor;
                cell->fade_index = grid->cells[old_idx].fade_index;
            } else {
                cell->glyph_index = rand() % NUM_STANDARD_GLYPHS;
                cell->glyph_cycle = randf();
                cell->brightness = 0.0f;
                cell->is_cursor = false;
                cell->fade_index = 0;
            }
        }
    }

    free(grid->column_time_offsets);
    free(grid->column_speed_offsets);
    free(grid->column_depths);
    free(grid->column_x_norm);
    free(grid->column_y_offset);
    free(grid->cells);

    grid->num_columns = new_columns;
    grid->num_rows = new_rows;
    grid->num_cells = new_cells_count;
    grid->column_time_offsets = new_time_offsets;
    grid->column_speed_offsets = new_speed_offsets;
    grid->column_depths = new_depths;
    grid->column_x_norm = new_x_norm;
    grid->column_y_offset = new_y_offset;
    grid->cells = new_cells;
}

void matrix_grid_update(MatrixGrid *grid, float delta_time) {
    if (!grid) return;

    grid->sim_time += delta_time;

    if (grid->volumetric) {
        for (int x = 0; x < grid->num_columns; x++) {
            grid->column_depths[x] += delta_time * grid->forward_speed;
            if (grid->column_depths[x] >= 1.0f) {
                grid->column_depths[x] = fmodf(grid->column_depths[x], 1.0f);
                grid->column_x_norm[x] = randf() * 2.4f - 1.2f;
                grid->column_y_offset[x] = (randf() - 0.5f) * 0.35f;
                grid->column_time_offsets[x] = randf() * 1000.0f;
            }
        }
    }

    for (int y = 0; y < grid->num_rows; y++) {
        for (int x = 0; x < grid->num_columns; x++) {
            int idx = y * grid->num_columns + x;
            MatrixCell *cell = &grid->cells[idx];

            float b = calculate_brightness(
                grid->sim_time,
                cell->column_time_offset,
                cell->column_speed_offset,
                x,
                y,
                grid->raindrop_length,
                grid->fall_speed,
                grid->slant
            );

            float b_below = calculate_brightness(
                grid->sim_time,
                cell->column_time_offset,
                cell->column_speed_offset,
                x,
                y + 1,
                grid->raindrop_length,
                grid->fall_speed,
                grid->slant
            );

            cell->brightness = b;

            /* The cursor is at the sawtooth wrap-around: tip of falling drop */
            bool cursor = (b > b_below) && (b > 0.70f);
            cell->is_cursor = cursor;

            /* Playdate fade index (0..31) */
            int fade = (int)(b * (float)NUM_FADES);
            if (fade < 0) fade = 0;
            if (fade >= NUM_FADES) fade = NUM_FADES - 1;
            cell->fade_index = fade;

            /* Glyph cycling: faster cycling when illuminated or cursor */
            float cycle_rate = grid->glyph_cycle_speed;
            if (cursor) {
                cycle_rate *= 4.0f;
            } else if (b > 0.5f) {
                cycle_rate *= 2.0f;
            }

            cell->glyph_cycle += delta_time * cycle_rate;
            if (cell->glyph_cycle >= 1.0f) {
                cell->glyph_cycle = fmodf(cell->glyph_cycle, 1.0f);
                int old_glyph = cell->glyph_index;
                int max_glyphs = grid->bonus_glyphs ? NUM_TOTAL_GLYPHS : NUM_STANDARD_GLYPHS;
                do {
                    cell->glyph_index = rand() % max_glyphs;
                } while (cell->glyph_index == old_glyph);
            }
        }
    }
}
