#include "Field.hpp"

#include <random>

static constexpr float  tile_size   {32.f};

Field::Field(const Size size)
    : size (size)
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
            std::size_t i {((r * grid[r].size()) + c) * 6};

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
    // determine which row and column the click was on
    auto relative_pos = click_pos - sf::Vector2i(vertex_grid[0].position);
    const auto click_row = relative_pos.y / static_cast<int>(tile_size);
    const auto click_col = relative_pos.x / static_cast<int>(tile_size);
    if (!been_clicked) {
        generate_field(click_row, click_col);
    }
}

void Field::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = &this->texture;

    target.draw(vertex_grid, states);
}

void Field::generate_field(const int click_row, const int click_col)
{
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

    for (std::size_t r {0}; r < grid.size(); ++r) {
        for (std::size_t c {0}; c < grid[r].size(); ++c) {
            std::size_t i {((r * grid[r].size()) + c) * 6};
            vertex_grid[i].position = sf::Vector2f{c * tile_size, r * tile_size};
            vertex_grid[i + 1].position = sf::Vector2f{(c + 1) * tile_size, r * tile_size};
            vertex_grid[i + 2].position = sf::Vector2f{c * tile_size, (r + 1) * tile_size};
            vertex_grid[i + 3].position = sf::Vector2f{c * tile_size, (r + 1) * tile_size};
            vertex_grid[i + 4].position = sf::Vector2f{(c + 1) * tile_size, r * tile_size};
            vertex_grid[i + 5].position = sf::Vector2f{(c + 1) * tile_size, (r + 1) * tile_size};

            // using weakly-typed enum to int conversion
            const Tile_type type = grid[r][c];
            vertex_grid[i].texCoords = sf::Vector2f{type * tile_size, 0};
            vertex_grid[i + 1].texCoords = sf::Vector2f{(type + 1) * tile_size, 0};
            vertex_grid[i + 2].texCoords = sf::Vector2f{type * tile_size, tile_size};
            vertex_grid[i + 3].texCoords = sf::Vector2f{type * tile_size, tile_size};
            vertex_grid[i + 4].texCoords = sf::Vector2f{(type + 1) * tile_size, 0};
            vertex_grid[i + 5].texCoords = sf::Vector2f{(type + 1) * tile_size, tile_size};
        }
    } 
    been_clicked = true;
}