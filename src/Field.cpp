#include "Field.hpp"

#include <random>

static constexpr float  tile_size   {32.f};

Field::Field(const Size size)
    : size {size}
{
    texture.loadFromFile("sprites.png");

    int num_rows {};
    int num_cols {};
    switch (this->size) {
    case small:
        num_rows = 9;
        num_cols = 9;
        break;
    case medium:
        num_rows = 16;
        num_cols = 16;
        break;
    case big:
        num_rows = 16;
        num_cols = 30;
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
            const auto i = coord_to_index(r, c);
            vertex_grid[i].position = sf::Vector2f{c * tile_size, r * tile_size};
            vertex_grid[i + 1].position = sf::Vector2f{(c + 1) * tile_size, r * tile_size};
            vertex_grid[i + 2].position = sf::Vector2f{c * tile_size, (r + 1) * tile_size};
            vertex_grid[i + 3].position = sf::Vector2f{c * tile_size, (r + 1) * tile_size};
            vertex_grid[i + 4].position = sf::Vector2f{(c + 1) * tile_size, r * tile_size};
            vertex_grid[i + 5].position = sf::Vector2f{(c + 1) * tile_size, (r + 1) * tile_size};

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

void Field::check_click(const sf::Vector2i click_pos)
{
    // check that the click is wtihin the bounds of the field
    // if this isn't done then clicking outside the field will result in indeing
    // out of the bounds of the array and the game crashes
    if (!vertex_grid.getBounds().contains(sf::Vector2f{click_pos})) {
        return;
    }

    // determine which row and column the click was on
    const auto relative_pos = click_pos - sf::Vector2i{vertex_grid[0].position};
    const auto click_row = relative_pos.y / static_cast<int>(tile_size);
    const auto click_col = relative_pos.x / static_cast<int>(tile_size);
    if (!been_clicked) {
        generate_field(click_row, click_col);
    }
    // Don't reveal the tile if the tile has been flagged.
    if (vertex_grid[coord_to_index(click_row, click_col)].texCoords.x == flag * tile_size) {
        return;
    }

    // if the player clicked on a mine, reveal the entire grid
    if (grid[click_row][click_col] == mine) {
        for (std::size_t r {0}; r < grid.size(); ++r) {
            for (std::size_t c {0}; c < grid[r].size(); ++c) {
                reveal(r, c);
            }
        }
        // set the bomb that was clicked on to be an explosion
        grid[click_row][click_col] = explosion;
        reveal(click_row, click_col);
    }
    else {
        reveal(click_row, click_col);
    }
}

void Field::place_flag(const sf::Vector2i click_pos)
{
    const auto relative_pos = click_pos - sf::Vector2i{vertex_grid[0].position};
    const auto click_row = relative_pos.y / static_cast<int>(tile_size);
    const auto click_col = relative_pos.x / static_cast<int>(tile_size);
    
    const auto i = coord_to_index(click_row, click_col);
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

void Field::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = &this->texture;

    target.draw(vertex_grid, states);
}

void Field::generate_field(const int click_row, const int click_col)
{
    // make sure that the field doesn't get generated again on the next click
    been_clicked = true;
    // randomly generate the position of the mines
    int num_mines {};
    switch (size) {
    case small:
        num_mines = 10;
        break;
    case medium:
        num_mines = 40;
        break;
    case big:
        num_mines = 99;
        break;
    };

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
    std::uniform_int_distribution<std::size_t> rand_col {0, grid.size() - 1};

    for (int i {0}; i < num_mines; ++i) {
        auto row {rand_row(mt)};
        auto col {rand_row(mt)};
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

            auto adjecent_mines = 0;
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
    const auto i = coord_to_index(click_row, click_col);
    const Tile_type type {grid[click_row][click_col]};

    auto is_revealed = [this](const int row, const int col) -> bool {
        const auto i = coord_to_index(row, col);
        return vertex_grid[i].texCoords.x != hidden * tile_size;
    };

    vertex_grid[i].texCoords.x = type * tile_size;
    vertex_grid[i + 1].texCoords.x = (type + 1) * tile_size;
    vertex_grid[i + 2].texCoords.x = type * tile_size;
    vertex_grid[i + 3].texCoords.x = type * tile_size;
    vertex_grid[i + 4].texCoords.x = (type + 1) * tile_size;
    vertex_grid[i + 5].texCoords.x = (type + 1) * tile_size;

    if (type != empty) {
        return;
    }

    // Reveal all adjacent tiles if this tile is empty.
    // If any of those adjacent tiles are empty then reveal those.
    for (auto r = click_row - 1; r < click_row + 2; ++r) {
        for (auto c = click_col - 1; c < click_col + 2; ++c) {
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

std::size_t Field::coord_to_index(const int row, const int col) const
{
    return ((row * grid[0].size()) + col) * 6;
}
