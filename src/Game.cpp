#include "Game.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
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
	SDL_SetRenderVSync(renderer, 1);

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

glm::vec2 playerPos;
glm::vec2 playerVelocity;

void Game::Setup() {
	playerPos = glm::vec2(10.0,10.0);
	playerVelocity = glm::vec2(1.0, 0.0);
}

void Game::Update(){
    // double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
    // millisecsPreviousFrame = SDL_GetTicks();

	playerPos.x += playerVelocity.x;
	playerPos.y += playerVelocity.y;
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// draw png texture
	SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);	
	SDL_DestroySurface(surface);
	SDL_FRect dstRect = {playerPos.x, playerPos.y, 32, 32};

	SDL_RenderTexture(renderer, texture, NULL, &dstRect);
	SDL_DestroyTexture(texture);

	SDL_RenderPresent(renderer);
}

void Game::Run(){
	Setup();
	while(isRunning) {
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
