#include "Game.h"
#include <iostream>

int main(int argc, char* argv[])
{
    
    Game game;

	// Initialize the game with a level file
    if (game.Initialize("Resources/level1.json")) {
        game.Run();
    }
    else {
        std::cout << "Game initialization failed. Press Enter to exit." << std::endl;
        std::cin.get();
    }

    game.Shutdown();

    return 0;
}

