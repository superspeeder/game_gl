#include "game/game.hpp"


#include <iostream>

int main() {
    try {
        game::Game game;
        game.mainloop();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
