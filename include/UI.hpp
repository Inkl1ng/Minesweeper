#pragma once

#include <SFML/Graphics.hpp>

class UI : public sf::Drawable {
public:
    UI(sf::RenderWindow& window);
    virtual ~UI();

private:
    virtual void    draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    sf::RenderWindow&   window;
    sf::RectangleShape  header_bar;
};
