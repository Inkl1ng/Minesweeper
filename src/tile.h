#ifndef SRC_TILE_H
#define SRC_TILE_H

#include "stdbool.h"
#include <raylib.h>

enum tile_type {
    EMPTY,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    MINE,
    EXPLODED,
    FLAGGED,
    HIDDEN,
    num_tile_types
};

struct tile {
    enum tile_type type;
    bool flagged;
    bool hidden;
};

// should only be called once at the start of the program
void load_tile_textures(void);

void tile_draw(struct tile* tile, int row, int col);

#endif
