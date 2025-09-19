#include "Game.hpp"
#include <iostream>
#include <exception>
#include <SFML/Graphics.hpp>

int main() {
    std::cout << "=== Space Defender - Enhanced Edition ===" << std::endl;
    std::cout << "SFML Version: " << SFML_VERSION_MAJOR << "."
              << SFML_VERSION_MINOR << "." << SFML_VERSION_PATCH << std::endl;
    
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
    
    std::cout << "=== Game Closed Successfully ===" << std::endl;
    return 0;
}