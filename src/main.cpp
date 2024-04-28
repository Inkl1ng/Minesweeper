#include <iostream>
#include "Game.hpp"

int main() {
    #ifndef NDEBUG
        std::cout << "*** DEBUG BUILD ***\n";
    #endif

    Game game;
    game.run();
    // Field field {Field::small};
    // field.generate_field(0, 0);

    return 0;
}
