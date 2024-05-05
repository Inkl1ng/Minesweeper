#include "UI.hpp"

#include <SFML/Graphics.hpp>

UI::UI(sf::RenderWindow& window)
    : window {window}
    , header_bar {}
{
    header_bar.setSize(sf::Vector2f{static_cast<float>(window.getSize().x), 64.f});
    header_bar.setFillColor(sf::Color{190, 190, 191});
    header_bar.setPosition(sf::Vector2f{0.f, 0.f});
    header_bar.setOutlineColor(sf::Color{60, 60, 60});
    header_bar.setOutlineThickness(-5.f);
}

UI::~UI()
{

}

void UI::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    window.draw(header_bar);
}