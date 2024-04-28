#include "Game.hpp"

#include <iostream>

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
    if (!mouse_pressed && lmb_clicked && window.hasFocus()) {
        field.check_click(sf::Mouse::getPosition(window));
        mouse_pressed = true;
    }
    else if (!lmb_clicked) {
        mouse_pressed = false;
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
