#include "./game/Game.h"
#include <sol/sol.hpp>
#include <iostream>

int main(int argc, char* argv[]) {

	Game game;

	game.initialize();
	game.run();
	game.destroy();

	return 0;
}
