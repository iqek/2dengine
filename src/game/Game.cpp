#include "Game.h"
#include "../ecs/ECS.h"
#include "../components/TransformComponent.h"
#include "../components/RigidbodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/AnimationComponent.h"
#include "../systems/MovementSystem.h"
#include "../systems/RenderSystem.h"
#include "../systems/AnimationSystem.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <iostream>
#include <fstream>

Game::Game() {
	isRunning = false;
	registry = std::make_unique<Registry>();
	resources = std::make_unique<ResourceManager>();
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

void Game::loadLevel(int level){
	// add the systems that need to be processed in our game
	registry->addSystem<MovementSystem>();
	registry->addSystem<RenderSystem>();
	registry->addSystem<AnimationSystem>();

	// adding assets to the ResourceManager
	resources->addTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	resources->addTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	resources->addTexture(renderer, "chopper-image", "./assets/images/chopper.png");
	resources->addTexture(renderer, "radar-image", "./assets/images/radar.png");
	resources->addTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");
	
	// load the tilemap
	int tileSize = 32;
	int mapCols = 25;
	int mapRows = 20;
	double tileScale = 3.0;
	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	if (!mapFile.is_open()) {
    	spdlog::error("Failed to open tilemap file");
    	return;
	}

	for(int y = 0; y < mapRows; y++){
		for(int x = 0; x < mapCols; x++){
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = registry->createEntity();
			tile.addComponent<TransformComponent>(
				glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
				glm::vec2(tileScale, tileScale)
			);
			tile.addComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);
		}
	}
	mapFile.close();

	// create an entity
	Entity tank = registry->createEntity();
	// add a component to the entity
	tank.addComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.addComponent<RigidbodyComponent>(glm::vec2(30.0, 0.0));
	tank.addComponent<SpriteComponent>("tank-image", 32, 32, 1);

	Entity truck = registry->createEntity();
	truck.addComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	truck.addComponent<RigidbodyComponent>(glm::vec2(20.0, 0.0));
	truck.addComponent<SpriteComponent>("truck-image", 32, 32, 1);

	Entity chopper = registry->createEntity();
	chopper.addComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.addComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
	chopper.addComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.addComponent<AnimationComponent>(2, 12, true);

	Entity radar = registry->createEntity();
	radar.addComponent<TransformComponent>(glm::vec2((windowWidth - 74), 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.addComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
	radar.addComponent<SpriteComponent>("radar-image", 64, 64, 2);
	radar.addComponent<AnimationComponent>(8, 5, true);
}

void Game::setup() {
	loadLevel(1);
}

void Game::update(){
	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
	millisecsPreviousFrame = SDL_GetTicks();

	// update the registry to process the entities that are waiting to be created/deleted
	registry->update();

	// invoke all systems that need to update
	registry->getSystem<MovementSystem>().update(deltaTime);
	registry->getSystem<AnimationSystem>().update();
}

void Game::render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// invoke all systems that need to render
	registry->getSystem<RenderSystem>().update(renderer, *resources);

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
