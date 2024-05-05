#include "Game.hpp"

#include <SFML/System.hpp>

Game::Game()
    : window {sf::VideoMode{512, 576}, "Minesweeper", sf::Style::Close}
    , field {Field::medium}
    , ui {window}
{
    // This comment is for the initializer list above.
    // The game is set to open up with a medium board and the window is sized
    // to fit a medium board.

    window.setKeyRepeatEnabled(false);
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
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                field.check_click(sf::Mouse::getPosition(window), sf::Mouse::Left);
            }
            else if (event.mouseButton.button == sf::Mouse::Right) {
                field.check_click(sf::Mouse::getPosition(window), sf::Mouse::Right);
            }
            else if (event.mouseButton.button == sf::Mouse::Middle) {
                field.check_click(sf::Mouse::getPosition(window), sf::Mouse::Middle);
            }
        }
    }
}

void Game::process_input()
{
}

void Game::update()
{

}

void Game::render()
{
    window.clear();

    window.draw(field);
    window.draw(ui);

    window.display();
}
