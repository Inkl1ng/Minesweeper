#include "Game.hpp"

#include <SFML/System.hpp>

Game::Game()
    : window {sf::VideoMode{1280, 960}, "Minesweeper", sf::Style::Close}
    , field {Field::small}
{
}

void Game::run()
{
    while (window.isOpen()) {
        poll_events();
        process_input();
        update();
        render();
    }
}

void Game::poll_events()
{
    sf::Event event {};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void Game::process_input()
{
    const bool lmb_clicked {sf::Mouse::isButtonPressed(sf::Mouse::Left)};
    // The part after the || is for laptop users who might not have an easy to
    // use right click. Also I don't think that SFML is registering alt + click
    // which is commonly used on laptops to input a RMB click.
    const bool rmb_clicked {sf::Mouse::isButtonPressed(sf::Mouse::Right)
        || lmb_clicked && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)};

    // This makes sures that only one action is done per mouse click.
    // Wihtout this, holding down LMB and dragging the mouse would keep reveal
    // every tile that the mouse goes over.
    if (mouse_pressed == true || !window.hasFocus()) {
        if (!lmb_clicked && !rmb_clicked) {
            mouse_pressed = false;
        }
        return;
    }
    else if (lmb_clicked || rmb_clicked) {
        field.check_click(sf::Mouse::getPosition(window), rmb_clicked);
        mouse_pressed = true;
    }
}

void Game::update()
{

}

void Game::render()
{
    window.clear();

    window.draw(field);

    window.display();
}
