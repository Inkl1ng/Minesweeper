#include "grid.h"

#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>

struct location {
    int row;
    int col;
};

static int count_adjacent_mines(struct grid* grid, int row, int col) {
    // This is needed becuase this function is also used to set the
    // sprite for tiles around mines.
    // This function returns MINE for tiles that are mines because
    // then it will set the correct sprite for the tile.
    if (grid->tiles[row * grid->cols + col].type == MINE) {
        return MINE;
    }

    int result = 0;
    struct location adjacents[8] = {
        { row - 1, col - 1 },
        { row, col - 1 },
        { row + 1, col - 1 },
        { row - 1, col },
        { row + 1, col },
        { row - 1, col + 1 },
        { row, col + 1 },
        { row + 1, col + 1 }
    };

    for (int i = 0; i < 8; ++i) { 
        // just creating these for ease of typing
        int r = adjacents[i].row; // row
        int c = adjacents[i].col; // column
        bool valid_loc = r >= 0 && r < grid->rows && c >= 0 && c < grid->cols;
        if (!valid_loc) {
            continue;
        } else if (grid->tiles[r * grid->cols + c].type == MINE) {
            ++result;
        }
    }

    return result;
}

// same thing as function above
static int count_adjacent_flags(struct grid* grid, int row, int col) {
    int result = 0;
    struct location adjacents[8] = {
        { row - 1, col - 1 },
        { row, col - 1 },
        { row + 1, col - 1 },
        { row - 1, col },
        { row + 1, col },
        { row - 1, col + 1 },
        { row, col + 1 },
        { row + 1, col + 1 }
    };

    for (int i = 0; i < 8; ++i) { 
        // just creating these for ease of typing
        int r = adjacents[i].row; // row
        int c = adjacents[i].col; // column
        bool valid_loc = r >= 0 && r < grid->rows && c >= 0 && c < grid->cols;
        if (!valid_loc) {
            continue;
        } else if (grid->tiles[r * grid->cols + c].flagged) {
            ++result;
        }
    }

    return result;
}

// revealing an empty tile reveals adjacent tiles.
// This function assumes that the location given is empty
static void reveal_empty(struct grid* grid, int row, int col)
{
    struct location adjacents[8] = {
        { row - 1, col - 1 },
        { row, col - 1 },
        { row + 1, col - 1 },
        { row - 1, col },
        { row + 1, col },
        { row - 1, col + 1 },
        { row, col + 1 },
        { row + 1, col + 1 }
    };

    for (int i = 0; i < 8; ++i) {
        int r = adjacents[i].row;
        int c = adjacents[i].col;
        bool valid_loc = r >= 0 && r < grid->rows && c >= 0 && c < grid->cols;
        if (!valid_loc) {
            continue;
        }

        struct tile* adj_tile = &grid->tiles[r * grid->cols + c];
        if (!adj_tile->hidden || adj_tile->flagged) {
            continue;
        }
        adj_tile->hidden = false;
        ++grid->tiles_revealed;

        if (adj_tile->type == EMPTY) {
            reveal_empty(grid, r, c);
        }
    }
}

// trigger loss state
static void explode(struct grid* grid)
{
    grid->exploded = true;
    // reveal location of all mines
    for (int r = 0; r < grid->rows; ++r) {
        for (int c = 0; c < grid->cols; ++c) {
            if (grid->tiles[r * grid->cols + c].type == MINE) {
                grid->tiles[r * grid->cols + c].hidden = false;
            }
        }
    }
}

//////////////////////////
// included from grid.h //
//////////////////////////

void grid_init(struct grid* grid)
{
    grid->size = MEDIUM;
    grid->tiles = malloc(sizeof(struct tile) * 256);
    grid_resize(grid, MEDIUM);
}

void grid_resize(struct grid* grid, enum grid_size size)
{
    grid->exploded = false;
    grid->tiles_revealed = 0;

    int num_mines;
    switch (size) {
    case SMALL:
        grid->rows = 9;
        grid->cols = 9;
        grid->flags_left = 10;
        num_mines = 10;
        break;
    case MEDIUM:
        grid->rows = 16;
        grid->cols = 16;
        grid->flags_left = 40;
        num_mines = 40;
        break;
    case BIG:
        grid->rows = 16;
        grid->cols = 30;
        grid->flags_left = 99;
        num_mines = 99;
        break;
    }

    // adding extra space to fit text
    SetWindowSize(grid->cols * 32, (grid->rows + 1) * 32);

    // initialize tiles
    // only need to reallocate memory if you are changing the grid size
    if (grid->size != size) {
        struct tile* old_ptr = realloc(grid->tiles,
                sizeof(struct tile) * grid->rows * grid->cols);
        if (old_ptr == NULL) {
            // error handling
        } else {
            grid->tiles = old_ptr;
            grid->size = size;
        }
    }
    for (int r = 0; r < grid->rows; ++r) {
        for (int c = 0; c < grid->cols; ++c) {
            grid->tiles[r * grid->cols + c].hidden = true;
            grid->tiles[r * grid->cols + c].flagged = false;
            grid->tiles[r * grid->cols + c].type = EMPTY;
        }
    }

    // randomize mines
    for (int i = 0; i < num_mines; ++i) {
        int row = rand() % grid->rows;
        int col = rand() % grid->cols;
        
        while (grid->tiles[row * grid->cols + col].type == MINE) {
            row = rand() % grid->rows;
            col = rand() % grid->cols;
        }

        grid->tiles[row * grid->cols + col].type = MINE;
    }
    
    // count adjecent mines
    for (int r = 0; r < grid->rows; ++r) {
        for (int c = 0; c < grid->cols; ++c) {
            int num = count_adjacent_mines(grid, r, c);
            grid->tiles[r * grid->cols + c].type = num;
        }
    }
}

void grid_draw(struct grid* grid)
{
    for (int r = 0; r < grid->rows; ++r) {
        for (int c = 0; c < grid->cols; ++c) {
            tile_draw(&grid->tiles[r * grid->cols + c], r, c);
        }
    }
}

void grid_reveal(struct grid* grid, int row, int col)
{
    struct tile* tile = &grid->tiles[row * grid->cols + col];
    if (!tile->hidden || tile->flagged) {
        return;
    }


    switch (tile->type) {
    case MINE:
        tile->type = EXPLODED;
        tile->hidden = false;
        explode(grid);
        break;
    case EMPTY:
        reveal_empty(grid, row, col);
        break;
    default:
        tile->hidden = false;
        ++grid->tiles_revealed;
        break;
    }
}

void grid_flag(struct grid* grid, int row, int col)
{    
    struct tile* tile = &grid->tiles[row * grid->cols + col];

    if (!tile->hidden) {
        return;
    }

    if (tile->flagged) {
        tile->flagged = false;
        ++grid->flags_left;
    } else if (grid->flags_left > 0) {
        tile->flagged = true;
        --grid->flags_left;
    }
}

void grid_chord(struct grid* grid, int row, int col)
{
    struct tile* tile = &grid->tiles[row * grid->cols + col];
    if (tile->hidden || tile->flagged) {
        return;
    }

    // empty tiles have special behavior when revealed
    if (tile->type == EMPTY) {
        reveal_empty(grid, row, col);
        return;
    }
    
    if (tile->type != count_adjacent_flags(grid, row, col)) {
        return;
    }

    struct location adjacents[8] = {
        { row - 1, col - 1 },
        { row, col - 1 },
        { row + 1, col - 1 },
        { row - 1, col },
        { row + 1, col },
        { row - 1, col + 1 },
        { row, col + 1 },
        { row + 1, col + 1 }
    };

    for (int i = 0; i < 8; ++i) {
        int r = adjacents[i].row;
        int c = adjacents[i].col;
        bool valid_loc = r >= 0 && r < grid->rows && c >= 0 && c < grid->cols;
        if (valid_loc) {
            grid_reveal(grid, r, c);
        }
    }
}

bool grid_check_win(struct grid* grid)
{
    if (grid->size == SMALL) {
        return grid->tiles_revealed == 71;
    } else if (grid->size == MEDIUM) {
        return grid->tiles_revealed == 216;
    } else { // grid->size == BIG
        return grid->tiles_revealed == 381;
    }
}
