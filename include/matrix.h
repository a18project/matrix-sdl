#ifndef MATRIX_SIM_H
#define MATRIX_SIM_H

#include <stdbool.h>
#include "config.h"

typedef struct {
    int x;
    int y;
    int glyph_index;
    float glyph_cycle;
    float brightness;
    bool is_cursor;
    int fade_index;
    float column_time_offset;
    float column_speed_offset;
} MatrixCell;

typedef struct {
    int num_columns;
    int num_rows;
    int num_cells;
    MatrixCell *cells;
    float *column_time_offsets;
    float *column_speed_offsets;
    float *column_depths;         /* 3D depth z in [0.0, 1.0] */
    float *column_x_norm;         /* 3D normalized x position */
    float *column_y_offset;       /* 3D vertical parallax phase */
    float sim_time;
    float raindrop_length;
    float fall_speed;
    float glyph_cycle_speed;
    float slant;
    float forward_speed;
    bool volumetric;
    bool bonus_glyphs;
} MatrixGrid;

MatrixGrid *matrix_grid_create(int num_columns, int num_rows, const AppConfig *cfg);
void matrix_grid_destroy(MatrixGrid *grid);
void matrix_grid_resize(MatrixGrid *grid, int new_columns, int new_rows);
void matrix_grid_update(MatrixGrid *grid, float delta_time);

#endif /* MATRIX_SIM_H */
