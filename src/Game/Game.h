#pragma once

#include <SDL3/SDL.h>

constexpr int FPS = 60;
constexpr int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {

	private:
		int millisecsPreviousFrame = 0;
		bool isRunning;
		SDL_Window* window;
		SDL_Renderer* renderer;
	public:
		Game();
		~Game();
		void initialize();
		void run();
		void setup();
		void processInput();
		void update();
		void render();
		void destroy();

		int windowWidth;
		int windowHeight;
};
