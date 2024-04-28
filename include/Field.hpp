#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class Field : public sf::Drawable {
public:
    enum Size {
        small,  // 9x9
        medium, // 16x16
        big     // 16x30
    };

            Field(Size size);
    virtual ~Field();

    void    check_click(sf::Vector2i click_pos);
    void    generate_field(int click_row, int click_col);

private:
    virtual void    draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void            reveal(sf::Vector2i click_pos);

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
        hidden,
        flag,
    };

    std::vector<std::vector<Tile_type>> grid            {};
    sf::VertexArray                     vertex_grid     {};
    Size                                size;
    sf::Texture                         texture         {};
    bool                                been_clicked    {false};
};