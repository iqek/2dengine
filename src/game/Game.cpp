#include "Game.h"
#include "../ecs/ECS.h"
#include "../components/TransformComponent.h"
#include "../components/RigidbodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../systems/MovementSystem.h"
#include "../systems/RenderSystem.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <iostream>

Game::Game() {
	isRunning = false;
	registry = std::make_unique<Registry>();
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
	// add the systems that need to be processed in our game
	registry->addSystem<MovementSystem>();
	registry->addSystem<RenderSystem>();

	// create an entity
	Entity cirno = registry->createEntity();
	// add a component to the entity
	cirno.addComponent<TransformComponent>(glm::vec2(10.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
	cirno.addComponent<RigidbodyComponent>(glm::vec2(50.0, 10.0));
	cirno.addComponent<SpriteComponent>(10, 10);

	Entity reimu = registry->createEntity();
	reimu.addComponent<TransformComponent>(glm::vec2(35.0, 30.0), glm::vec2(1.0, 1.0), 0.0);
	reimu.addComponent<RigidbodyComponent>(glm::vec2(20.0, 90.0));
	reimu.addComponent<SpriteComponent>(10, 50);
}

void Game::update(){
	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
	millisecsPreviousFrame = SDL_GetTicks();

	// update the registry to process the entities that are waiting to be created/deleted
	registry->update();

	// invoke all systems that need to update
	registry->getSystem<MovementSystem>().update(deltaTime);
}

void Game::render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// invoke all systems that need to render
	registry->getSystem<RenderSystem>().update(renderer);

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
