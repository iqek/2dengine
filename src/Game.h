#pragma once

#include <SDL3/SDL.h>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 /FPS;

class Game {
	
	private:
		// int millisecsPreviousFrame = 0;
		bool isRunning;
		SDL_Window* window;
		SDL_Renderer* renderer;
	public:
		Game();
		~Game();
		void Initialize();
		void Run();
		void Setup();
		void ProcessInput();
		void Update();
		void Render();
		void Destroy();
		
		int windowWidth;
		int windowHeight;
};
