#include <iostream>
#include "Game.hpp"

int main() {
    #ifndef NDEBUG
        std::cout << "*** DEBUG BUILD ***\n";
    #endif

    Game game;
    game.run();

    return 0;
}
