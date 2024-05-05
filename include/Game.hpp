#pragma once

#include <SFML/Graphics.hpp>
#include "Field.hpp"
#include "UI.hpp"

class Game {
public:
    Game();

    void    run();

private:
    void    poll_events();
    void    process_input(); 
    void    update();
    void    render();

private:
    sf::RenderWindow    window;

    Field   field;
    UI      ui;
    
    bool    mouse_pressed   {false};
};
