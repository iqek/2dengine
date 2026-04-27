#include "Game.h"
#include <SDL3/SDL.h>
#include <iostream>

Game::Game() {
	isRunning = false;
	std::cout << "Game constructor called \n";
}

Game::~Game(){
	std::cout << "Game destructor called \n";
}

void Game::Initialize(){
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
		std::cerr << "error initializing SDL: " << SDL_GetError() << std::endl;
		return;
	}
	
	const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
	windowWidth = displayMode->w;
	windowHeight = displayMode->h;

	window = SDL_CreateWindow(NULL, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS );

	if (!window) {
		std::cerr << "error creating sdl window: " << SDL_GetError() << std::endl;
		return;
	}

	renderer = SDL_CreateRenderer(window, NULL );

	if (!renderer){
		std::cerr << "error creating sdl renderer: " << SDL_GetError() << std::endl;
		return;
	}
	
	SDL_SetWindowFullscreen(window, true);

	isRunning = true;
}

void Game::ProcessInput(){
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)){
		switch(sdlEvent.type) {
			case SDL_EVENT_QUIT: 
				isRunning = false;
				break;
			case SDL_EVENT_KEY_DOWN:
				if (sdlEvent.key.key == SDLK_ESCAPE) {
					isRunning = false;
				}
				break;
		}	
	}
}

void Game::Update(){
	// update game objects ...	
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);

	// render all game objects ...

	SDL_RenderPresent(renderer);
}

void Game::Run(){
	 while(isRunning){
		ProcessInput();
		Update();
		Render();
	}
}

void Game::Destroy(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();	
}
