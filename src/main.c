#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "grid.h"
#include "tile.h"

int main()
{
    // creating a consistent seed for debugging
#ifndef NDEBUG
    srand(0);
#else
    srand(time(NULL));
#endif

    // extra 32 is for flags text
    InitWindow(288, 288 + 32, "Minesweeper");

    load_tile_textures();

    struct grid grid;
    grid_init(&grid);
    grid_resize(&grid, SMALL);

    int previous_num_flags = grid.flags_left;
    bool won_game = false;
    char ui_text[15];

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE)) {
            grid_resize(&grid, SMALL);
            won_game = false;
        } else if (IsKeyPressed(KEY_TWO)) {
            grid_resize(&grid, MEDIUM);
            won_game = false;
        } else if (IsKeyPressed(KEY_THREE)) {
            grid_resize(&grid, BIG);
            won_game = false;
        }

        int row = (GetMouseY() / 32) - 1;
        int col = GetTouchX() / 32;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            grid_reveal(&grid, row, col);
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            grid_flag(&grid, row, col);
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
            grid_chord(&grid, row, col);
        }

        if (previous_num_flags != grid.flags_left) {
            previous_num_flags = grid.flags_left;
            snprintf(ui_text, 15, "Flags left: %i", grid.flags_left);
        } else if (won_game != grid_check_win(&grid)) {
            won_game = true;
            snprintf(ui_text, 15, "You won!");
        }

        BeginDrawing();

        ClearBackground(BLACK);

        grid_draw(&grid);

        // display remaining flags
        DrawText(ui_text, 4, 2, 28, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
