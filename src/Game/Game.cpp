#include "Game.h"
#include "../ECS/ECS.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <iostream>

Game::Game() {
	isRunning = false;
	spdlog::info("Game constructor called");
}

Game::~Game(){
	spdlog::info("Game destructor called");
}

void Game::initialize(){
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
		spdlog::error("Error initializing SDL");
		return;
	}

	const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
	windowWidth = displayMode->w;
	windowHeight = displayMode->h;

	window = SDL_CreateWindow(NULL, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);

	if (!window) {
		spdlog::error("Error creating SDL window");
		return;
	}

	renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetRenderVSync(renderer, 1);

	if (!renderer){
		spdlog::error("Error creating SDL renderer");
		return;
	}

	SDL_SetWindowFullscreen(window, true);

	isRunning = true;
}

void Game::processInput(){
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

void Game::setup() {

}

void Game::update(){
	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
	millisecsPreviousFrame = SDL_GetTicks();
}

void Game::render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	SDL_RenderPresent(renderer);
}

void Game::run(){
	setup();
	while(isRunning) {
		processInput();
		update();
		render();
	}
}

void Game::destroy(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
