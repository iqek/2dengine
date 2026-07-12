#include "Game.h"
#include "../ecs/ECS.h"
#include "../components/TransformComponent.h"
#include "../components/RigidbodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/AnimationComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../systems/MovementSystem.h"
#include "../systems/RenderSystem.h"
#include "../systems/AnimationSystem.h"
#include "../systems/CollisionSystem.h"
#include "../systems/DamageSystem.h"
#include "../systems/KeyboardControlSystem.h"
#include "../systems/DebugRenderSystem.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <iostream>
#include <fstream>

Game::Game() {
	isRunning = false;
	isDebug = false;
	registry = std::make_unique<Registry>();
	resources = std::make_unique<ResourceManager>();
	eventBus = std::make_unique<EventBus>();
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
				if(sdlEvent.key.key == SDLK_P){
					isDebug = !isDebug;
				}
				eventBus->emitEvent<KeyPressedEvent>(sdlEvent.key.key);
				break;
		}
	}
}

void Game::loadLevel(int level){
	// add the systems that need to be processed in our game
	registry->addSystem<MovementSystem>();
	registry->addSystem<RenderSystem>();
	registry->addSystem<AnimationSystem>();
	registry->addSystem<CollisionSystem>();
	registry->addSystem<DamageSystem>();
	registry->addSystem<KeyboardControlSystem>();
	registry->addSystem<DebugRenderSystem>();

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
	Entity chopper = registry->createEntity();
	// add a component to the entity
	chopper.addComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.addComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
	chopper.addComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.addComponent<AnimationComponent>(2, 12, true);
	
	Entity tank = registry->createEntity();
	tank.addComponent<TransformComponent>(glm::vec2(500.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.addComponent<RigidbodyComponent>(glm::vec2(-30.0, 0.0));
	tank.addComponent<SpriteComponent>("tank-image", 32, 32, 1);
	tank.addComponent<BoxColliderComponent>(32, 32, glm::vec2(0));

	Entity truck = registry->createEntity();
	truck.addComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	truck.addComponent<RigidbodyComponent>(glm::vec2(20.0, 0.0));
	truck.addComponent<SpriteComponent>("truck-image", 32, 32, 1);
	truck.addComponent<BoxColliderComponent>(32, 32, glm::vec2(0));

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

	// reset al event handlers for the current frame
	eventBus->reset();
	// perform the subscription of events for all systems
	registry->getSystem<DamageSystem>().listenToEvents(eventBus.get());
	registry->getSystem<KeyboardControlSystem>().listenToEvents(eventBus.get());
	// improvement: subscribe once when a system/entity is created, unsubscribe when it's destroyed so that this doesn't get called every update

	// update the registry to process the entities that are waiting to be created/deleted
	registry->update();

	// invoke all systems that need to update
	registry->getSystem<MovementSystem>().update(deltaTime);
	registry->getSystem<AnimationSystem>().update();
	registry->getSystem<CollisionSystem>().update(eventBus.get());
}

void Game::render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// invoke all systems that need to render
	registry->getSystem<RenderSystem>().update(renderer, resources.get());
	if(isDebug){
		registry->getSystem<DebugRenderSystem>().update(renderer);
	}
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
