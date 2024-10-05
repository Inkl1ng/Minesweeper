#include "tile.h"

#include <raylib.h>

static Texture textures[num_tile_types];

//////////////////////////
// included from tile.h //
//////////////////////////

void load_tile_textures(void) {
    textures[EMPTY] = LoadTexture(ASSET_PATH "/empty.png");
    textures[ONE] = LoadTexture(ASSET_PATH "/one.png");
    textures[TWO] = LoadTexture(ASSET_PATH "/two.png");
    textures[THREE] = LoadTexture(ASSET_PATH "/three.png");
    textures[FOUR] = LoadTexture(ASSET_PATH "/four.png");
    textures[FIVE] = LoadTexture(ASSET_PATH "/five.png");
    textures[SIX] = LoadTexture(ASSET_PATH "/six.png");
    textures[SEVEN] = LoadTexture(ASSET_PATH "/seven.png");
    textures[EIGHT] = LoadTexture(ASSET_PATH "/eight.png");
    textures[MINE] = LoadTexture(ASSET_PATH "/mine.png");
    textures[EXPLODED] = LoadTexture(ASSET_PATH "/exploded.png");
    textures[FLAGGED] = LoadTexture(ASSET_PATH "/flagged.png");
    textures[HIDDEN] = LoadTexture(ASSET_PATH "/hidden.png");
}

void tile_draw(struct tile* tile, int row, int col) {
    Texture texture;
    if (tile->flagged) {
        texture = textures[FLAGGED];
    } else if (tile->hidden) {
        texture = textures[HIDDEN];
    }  else {
        // enum to int conversion
        texture = textures[tile->type];
    }

    // shifting the tiles down to make room for text
    DrawTexture(texture, col * 32, (row + 1) * 32, WHITE);
}
