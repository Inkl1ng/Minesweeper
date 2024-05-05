#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Field : public sf::Drawable {
public:
    enum Size {
        small,  // 9x9
        medium, // 16x16
        big     // 16x30
    };

    Field(Size size);
    virtual ~Field();

    void    check_click(sf::Vector2i click_pos, sf::Mouse::Button click_type);
    void    generate_field(int click_row, int click_col);

private:
    enum Tile_type {
        // going to use enum to integer conversion to set texture coordinates
        empty,
        one,
        two,
        three,
        four,
        five,
        six,
        seven,
        eight,
        mine,
        explosion,
        hidden,
        flag,
    };

    virtual void    draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void    reveal(int click_row, int click_col);
    void    place_flag(int click_row, int click_col);
    void    chord(int click_row, int click_col);

    // helper methods
    bool    is_within_grid(int row, int col) const;
    bool    is_displayed_as(int row, int col, Tile_type type) const;

    std::size_t coord_to_index(int row, int col) const;

private:

    std::vector<std::vector<Tile_type>> grid;

    sf::VertexArray vertex_grid;
    sf::Texture     texture;

    Size    size;
    bool    been_clicked    {false};
    bool    mine_exploded   {false};
    int     tiles_revealed  {0};

    int num_mines;
    int num_rows;
    int num_cols;
};
