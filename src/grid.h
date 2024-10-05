#ifndef SRC_GRID_H
#define SRC_GRID_H

#include <stdbool.h>
#include "tile.h"

enum grid_size {
    SMALL,
    MEDIUM,
    BIG
};

struct grid {
    struct tile* tiles;
    enum grid_size size;
    int rows;
    int cols;
    bool exploded;
    int flags_left;
    int tiles_revealed;
};

void grid_init(struct grid* grid);
void grid_resize(struct grid* grid, enum grid_size size);

void grid_draw(struct grid* grid);

void grid_reveal(struct grid* grid, int row, int col);
void grid_flag(struct grid* grid, int row, int col);
void grid_chord(struct grid* grid, int row, int col);

bool grid_check_win(struct grid* grid);

#endif

