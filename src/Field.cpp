#include "Field.hpp"
#include "SFML/System/Vector2.hpp"

#include <random>

static constexpr float  tile_size   {32.f};

Field::Field(const Size size)
    : size {size}
{
    texture.loadFromFile("sprites.png");

    switch (this->size) {
    case small:
        num_rows = 9;
        num_cols = 9;
        num_mines = 10;
        break;
    case medium:
        num_rows = 16;
        num_cols = 16;
        num_mines = 40;
        break;
    case big:
        num_rows = 16;
        num_cols = 30;
        num_mines = 99;
        break;
    }

    // initialize the grid
    grid.resize(num_rows);
    for (auto& col : grid) {
        col.resize(num_cols);
        for (auto& tile : col) {
            tile = hidden;
        }
    }

    vertex_grid.setPrimitiveType(sf::Triangles);
    // 1 tile = 2 triangles = 3 verteicies * 2 = 6 verticies
    // multiplying num_rows by num_cols to represent a 2d grid in a 1d vector
    vertex_grid.resize(num_rows * num_cols * 6);

    // start the grid out as all hidden tiles
    for (std::size_t r{0}; r < grid.size(); ++r) {
        for (std::size_t c {0}; c < grid[r].size(); ++c) {
            const std::size_t i = coord_to_index(r, c);
            vertex_grid[i].position = sf::Vector2f{c * tile_size, r * tile_size};
            vertex_grid[i + 1].position = sf::Vector2f{(c + 1) * tile_size, r * tile_size};
            vertex_grid[i + 2].position = sf::Vector2f{c * tile_size, (r + 1) * tile_size};
            vertex_grid[i + 3].position = sf::Vector2f{c * tile_size, (r + 1) * tile_size};
            vertex_grid[i + 4].position = sf::Vector2f{(c + 1) * tile_size, r * tile_size};
            vertex_grid[i + 5].position = sf::Vector2f{(c + 1) * tile_size, (r + 1) * tile_size};

            vertex_grid[i].position.y += 64.f;
            vertex_grid[i + 1].position.y += 64.f;
            vertex_grid[i + 2].position.y += 64.f;
            vertex_grid[i + 3].position.y += 64.f;
            vertex_grid[i + 4].position.y += 64.f;
            vertex_grid[i + 5].position.y += 64.f;

            vertex_grid[i].texCoords = sf::Vector2f{hidden * tile_size, 0};
            vertex_grid[i + 1].texCoords = sf::Vector2f{(hidden + 1) * tile_size, 0};
            vertex_grid[i + 2].texCoords = sf::Vector2f{hidden * tile_size, tile_size};
            vertex_grid[i + 3].texCoords = sf::Vector2f{hidden * tile_size, tile_size};
            vertex_grid[i + 4].texCoords = sf::Vector2f{(hidden + 1) * tile_size, 0};
            vertex_grid[i + 5].texCoords = sf::Vector2f{(hidden + 1) * tile_size, tile_size};
        }
    }
}

Field::~Field()
{

}

void Field::check_click(const sf::Vector2i click_pos, const sf::Mouse::Button click_type)
{
    // check that the click is wtihin the bounds of the field
    // if this isn't done then clicking outside the field will result in indeing
    // out of the bounds of the array and the game crashes
    if (!vertex_grid.getBounds().contains(sf::Vector2f{click_pos})) {
        return;
    }

    // determine which row and column the click was on
    const sf::Vector2i relative_pos {click_pos - sf::Vector2i{vertex_grid[0].position}};
    const int click_row {relative_pos.y / static_cast<int>(tile_size)};
    const int click_col {relative_pos.x / static_cast<int>(tile_size)};

    const bool is_number {grid[click_row][click_col] >= one
        && grid[click_row][click_col] <= eight};
    
    if (click_type == sf::Mouse::Right) {
            place_flag(click_row, click_col);
        return;
    }
    else if (is_number && click_type == sf::Mouse::Middle
            && !is_displayed_as(click_row, click_col, hidden)
            && !is_displayed_as(click_row, click_col, flag)) {
        chord(click_row, click_col);
        return;
    }
    else if (click_type == sf::Mouse::Left && !been_clicked) {
        generate_field(click_row, click_col);
    }
    // At this point, any inputs that aren't left clicks shouldn't be processed.
    else if (click_type != sf::Mouse::Left) {
        return;
    }
    // Don't reveal the tile if the tile has been flagged.
    if (vertex_grid[coord_to_index(click_row, click_col)].texCoords.x == flag * tile_size) {
        return;
    }

    // if the player clicked on a mine, reveal the entire grid
    if (grid[click_row][click_col] == mine && !mine_exploded) {
        // Blow up the mine that was clicked on.
        grid[click_row][click_col] = explosion;
        for (std::size_t r {0}; r < grid.size(); ++r) {
            for (std::size_t c {0}; c < grid[r].size(); ++c) {
                reveal(r, c);
            }
        }
        mine_exploded = true;
    }
    else {
        reveal(click_row, click_col);
    }
}

void Field::place_flag(const int click_row, const int click_col)
{
    const std::size_t i {coord_to_index(click_row, click_col)};
    // Get the type of tile displayed at the index
    const Tile_type displayed {static_cast<Tile_type>(vertex_grid[i].texCoords.x / tile_size)};
    // Can't place flags on tiles that are already revealed so don't do
    // anything in that case.
    // If the tile is already a flag, then remove the flag
    if (displayed == flag) {
        vertex_grid[i].texCoords.x = hidden * tile_size;
        vertex_grid[i + 1].texCoords.x = (hidden + 1) * tile_size;
        vertex_grid[i + 2].texCoords.x = hidden * tile_size;
        vertex_grid[i + 3].texCoords.x = hidden * tile_size;
        vertex_grid[i + 4].texCoords.x = (hidden + 1) * tile_size;
        vertex_grid[i + 5].texCoords.x = (hidden + 1) * tile_size;
    }
    // Switch the texture to be a flag.
    // Only switching the X-coordinate because the Y-coordinate doesn't change
    else if (displayed == hidden) {
        vertex_grid[i].texCoords.x = flag * tile_size;
        vertex_grid[i + 1].texCoords.x = (flag + 1) * tile_size;
        vertex_grid[i + 2].texCoords.x = flag * tile_size;
        vertex_grid[i + 3].texCoords.x = flag * tile_size;
        vertex_grid[i + 4].texCoords.x = (flag + 1) * tile_size;
        vertex_grid[i + 5].texCoords.x = (flag + 1) * tile_size;
    }
}

void Field::chord(const int click_row, const int click_col) {
    // Count the number of mines around the tile that was clicked.
    int adjecent_flags {0};

    auto is_flagged = [this](const int r, const int c) -> bool {
        return vertex_grid[coord_to_index(r, c)].texCoords.x == flag * tile_size;
    };

    // No need to account for the tile that was chorded on because it is
    // guaranteed to not be a flag.
    for (int r {click_row - 1}; r < click_row + 2; ++r) {
        for (int c {click_col - 1}; c < click_col + 2; ++c) {
            if (is_within_grid(r, c) && is_flagged(r, c)) {
                ++adjecent_flags;
            }
        }
    }
    // Chording is only allowed the number of adjacent flags is equal to the
    // number on the tile.
    if (adjecent_flags != grid[click_row][click_col]) {
        return;
    }

    // Reveal all adjacent tiles.
    for (int r {click_row - 1}; r < click_row + 2; ++r) {
        for (int c {click_col - 1}; c < click_col + 2; ++c) {
            if (!is_within_grid(r, c)) {
                continue;
            }
            // Explode any mines that were revealed.
            if (grid[r][c] == mine) {
                // int version of tile_size needed for this part of the code.
                constexpr int tile_size_i = static_cast<int>(tile_size);
                check_click({c * tile_size_i, (r * tile_size_i) + 64}, sf::Mouse::Button::Left);
            }
            else if (!is_flagged(r, c)) {
                reveal(r, c);
            }
        }
    }
}


void Field::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = &this->texture;

    target.draw(vertex_grid, states);
}

void Field::generate_field(const int click_row, const int click_col)
{
    // make sure that the field doesn't get generated again on the next click
    been_clicked = true;

    for (auto& col : grid) {
        for (auto& tile : col) {
            tile = empty;
        }
    }

    std::mt19937 mt {std::random_device{}()};
    // using size_t because grid.size() returns a size_t
    // also we are only using getting positive numbers so size_t doesn't matter
    // it does make this code look kinda uglier but that's alright
    std::uniform_int_distribution<std::size_t> rand_row {0, grid.size() - 1};
    std::uniform_int_distribution<std::size_t> rand_col {0, grid[0].size() - 1};

    for (int i {0}; i < num_mines; ++i) {
        std::size_t row {rand_row(mt)};
        std::size_t col {rand_col(mt)};
        while (row == click_row && col == click_col) {
            row = rand_row(mt);
            col = rand_row(mt);
        }

        grid[row][col] = mine;
    }

    // includes bounds checking
    auto is_mine = [this](std::size_t row, std::size_t col) -> bool {
        if (this->is_within_grid(row, col)) {
            return grid[row][col] == mine;
        }
        else {
            return false;
        }
    };

    for (std::size_t r {0}; r < grid.size(); ++r) {
        for (std::size_t c {0}; c < grid[r].size(); ++c) {
            // don't count adjecent mines for a tile that is a mine
            if (grid[r][c] == mine) {
                continue;
            }

            int adjecent_mines = 0;
            // this looks ugly but I don't know any other way to do this
            // goes through each adjecent spot and sees if it is a mine
            if (is_mine(r - 1, c - 1)) {
                ++adjecent_mines;
            }
            if (is_mine(r - 1, c)) {
                ++adjecent_mines;
            }
            if (is_mine(r - 1, c + 1)) {
                ++adjecent_mines;
            }
            if (is_mine(r, c - 1)) {
                ++adjecent_mines;
            }
            if (is_mine(r, c + 1)) {
                ++adjecent_mines;
            }
            if (is_mine(r + 1, c - 1)) {
                ++adjecent_mines;
            }
            if (is_mine(r + 1, c)) {
                ++adjecent_mines;
            }
            if (is_mine(r + 1, c + 1)) {
                ++adjecent_mines;
            }
            grid[r][c] = static_cast<Tile_type>(adjecent_mines);
        }
    }
}

void Field::reveal(const int click_row, const int click_col)
{
    ++tiles_revealed;
    const std::size_t i = coord_to_index(click_row, click_col);
    const Tile_type type {grid[click_row][click_col]};

    auto is_revealed = [this](const int row, const int col) -> bool {
        const std::size_t i = coord_to_index(row, col);
        return vertex_grid[i].texCoords.x != hidden * tile_size;
    };

    if (!is_revealed(click_row, click_col) || type == explosion) {
        vertex_grid[i].texCoords.x = type * tile_size;
        vertex_grid[i + 1].texCoords.x = (type + 1) * tile_size;
        vertex_grid[i + 2].texCoords.x = type * tile_size;
        vertex_grid[i + 3].texCoords.x = type * tile_size;
        vertex_grid[i + 4].texCoords.x = (type + 1) * tile_size;
        vertex_grid[i + 5].texCoords.x = (type + 1) * tile_size;
    }

    if (type != empty) {
        return;
    }

    // Reveal all adjacent tiles if this tile is empty.
    // If any of those adjacent tiles are empty then reveal those.
    for (int r {click_row - 1}; r < click_row + 2; ++r) {
        for (int c {click_col - 1}; c < click_col + 2; ++c) {
            if (is_within_grid(r, c) && !is_revealed(r, c)) {
                reveal(r, c);
            }
        }
    }
}

bool Field::is_within_grid(const int row, const int col) const
{
    return row >= 0 && row < grid.size() && col >= 0 && col < grid[row].size();
}

bool Field::is_displayed_as(const int row, const int col, const Tile_type type) const
{
    std::size_t i {coord_to_index(row, col)};
    return vertex_grid[i].texCoords.x == type * tile_size;
}

std::size_t Field::coord_to_index(const int row, const int col) const
{
    return ((row * grid[0].size()) + col) * 6;
}

