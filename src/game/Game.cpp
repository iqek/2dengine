#include "Game.h"
#include "../ecs/ECS.h"
#include "../components/TransformComponent.h"
#include "../components/RigidbodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/AnimationComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../components/HealthComponent.h"
#include "../components/KeyboardControlledComponent.h"
#include "../components/CameraFollowComponent.h"
#include "../components/ProjectileEmitterComponent.h"
#include "../components/TextLabelComponent.h"
#include "../systems/MovementSystem.h"
#include "../systems/CameraMovementSystem.h"
#include "../systems/RenderSystem.h"
#include "../systems/RenderTextSystem.h"
#include "../systems/AnimationSystem.h"
#include "../systems/CollisionSystem.h"
#include "../systems/DamageSystem.h"
#include "../systems/ProjectileEmitSystem.h"
#include "../systems/ProjectileLifecycleSystem.h"
#include "../systems/KeyboardControlSystem.h"
#include "../systems/DebugRenderSystem.h"
#include "../systems/HealthBarRenderSystem.h"
#include "../systems/RenderGUISystem.h"
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>
#include <iostream>
#include <fstream>

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game()
	: isRunning(false),
	  isDebug(false),
	  registry(std::make_unique<Registry>()),
	  resources(std::make_unique<ResourceManager>()),
	  eventBus(std::make_unique<EventBus>())
{
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

	if (!TTF_Init()) {
		spdlog::error("Error initializing ttf");
		return;
	}

	const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
	windowWidth = displayMode->w;
	windowHeight = displayMode->h;

	window = SDL_CreateWindow(nullptr, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);

	if (!window) {
		spdlog::error("Error creating SDL window");
		return;
	}

	renderer = SDL_CreateRenderer(window, nullptr);
	SDL_SetRenderVSync(renderer, 1);

	if (!renderer){
		spdlog::error("Error creating SDL renderer");
		return;
	}

	// Initialize the ImGui context
	ImGui::CreateContext();
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	// Initialize the camera view with the entire screen area
	camera.x = 0;
	camera.y = 0;
	camera.w = windowWidth;
	camera.h = windowHeight;

	SDL_SetWindowFullscreen(window, true);
	isRunning = true;
}

void Game::processInput(){
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)){
		// ImGui SDL input
		ImGui_ImplSDL3_ProcessEvent(&sdlEvent);
		ImGuiIO& io = ImGui::GetIO(); (void) io;
		float mouseX, mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
		io.MousePos = ImVec2(mouseX, mouseY);
		io.MouseDown[0] = buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);

		// Handle core SDL events
		switch(sdlEvent.type) {
			case SDL_EVENT_QUIT:
				isRunning = false;
				break;
			case SDL_EVENT_KEY_DOWN:
				if (sdlEvent.key.key == SDLK_ESCAPE) {
					isRunning = false;
				}
				if(sdlEvent.key.key == SDLK_D){
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
	registry->addSystem<CameraMovementSystem>();
	registry->addSystem<ProjectileEmitSystem>();
	registry->addSystem<ProjectileLifecycleSystem>();
	registry->addSystem<RenderTextSystem>();
	registry->addSystem<HealthBarRenderSystem>();
	registry->addSystem<RenderGUISystem>();

	// adding assets to the ResourceManager
	resources->addTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	resources->addTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	resources->addTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
	resources->addTexture(renderer, "radar-image", "./assets/images/radar.png");
	resources->addTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");
	resources->addTexture(renderer, "bullet-image", "./assets/images/bullet.png");

	resources->addFont("charriot-font-30", "./assets/fonts/charriot.ttf", 30);
	resources->addFont("pico8-font-5", "./assets/fonts/pico8.ttf", 5);
	resources->addFont("pico8-font-10", "./assets/fonts/pico8.ttf", 10);

	// load the tilemap
	int tileSize = 32;
	int mapCols = 25;
	int mapRows = 20;
	float tileScale = 3.0f;
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
			tile.group("tiles");
			tile.addComponent<TransformComponent>(
				glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
				glm::vec2(tileScale, tileScale)
			);
			tile.addComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, false, srcRectX, srcRectY);
		}
	}
	mapFile.close();
	mapWidth = static_cast<int>(mapCols * tileSize * tileScale);
	mapHeight = static_cast<int>(mapRows * tileSize * tileScale);

	// create an entity
	Entity chopper = registry->createEntity();
	chopper.tag("player");
	// add components to the entity
	chopper.addComponent<TransformComponent>(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	chopper.addComponent<RigidbodyComponent>(glm::vec2(0.0f, 0.0f));
	chopper.addComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.addComponent<AnimationComponent>(2, 12, true);
	chopper.addComponent<BoxColliderComponent>(32, 32);
	chopper.addComponent<ProjectileEmitterComponent>(glm::vec2(150.0f, 150.0f), 0, 10000, 10, true);
	chopper.addComponent<KeyboardControlledComponent>(glm::vec2(0, -80), glm::vec2(80, 0), glm::vec2(0, 80), glm::vec2(-80, 0));
	chopper.addComponent<CameraFollowComponent>();
	chopper.addComponent<HealthComponent>(100);

	Entity tank = registry->createEntity();
	tank.group("enemies");
	tank.addComponent<TransformComponent>(glm::vec2(500.0f, 200.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	tank.addComponent<RigidbodyComponent>(glm::vec2(0.0f, 0.0f));
	tank.addComponent<SpriteComponent>("tank-image", 32, 32, 1);
	tank.addComponent<BoxColliderComponent>(32, 32);
	tank.addComponent<ProjectileEmitterComponent>(glm::vec2(100.0f, 0.0f), 5000, 3000, 10, false);
	tank.addComponent<HealthComponent>(100);

	Entity truck = registry->createEntity();
	truck.group("enemies");
	truck.addComponent<TransformComponent>(glm::vec2(10.0f, 200.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	truck.addComponent<RigidbodyComponent>(glm::vec2(0.0f, 0.0f));
	truck.addComponent<SpriteComponent>("truck-image", 32, 32, 1);
	truck.addComponent<BoxColliderComponent>(32, 32);
	truck.addComponent<ProjectileEmitterComponent>(glm::vec2(0.0f, 100.0f), 2000, 5000, 10, false);
	truck.addComponent<HealthComponent>(100);

	Entity radar = registry->createEntity();
	radar.addComponent<TransformComponent>(glm::vec2(static_cast<float>(windowWidth - 74), 10.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	radar.addComponent<RigidbodyComponent>(glm::vec2(0.0f, 0.0f));
	radar.addComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
	radar.addComponent<AnimationComponent>(8, 5, true);

	Entity label = registry->createEntity();
	label.addComponent<TransformComponent>(glm::vec2(windowWidth/2 - 40, 10.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	SDL_Color white = { 255, 255, 255, 255 };
	label.addComponent<TextLabelComponent>("CHOPPER 1.0", "charriot-font-30", white, true);
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
	registry->getSystem<ProjectileEmitSystem>().listenToEvents(eventBus.get());
	// improvement: subscribe once when a system/entity is created, unsubscribe when it's destroyed so that this doesn't get called every update

	// update the registry to process the entities that are waiting to be created/deleted
	registry->update();

	// invoke all systems that need to update
	registry->getSystem<MovementSystem>().update(deltaTime);
	registry->getSystem<CameraMovementSystem>().update(camera);
	registry->getSystem<AnimationSystem>().update();
	registry->getSystem<ProjectileLifecycleSystem>().update();
	registry->getSystem<ProjectileEmitSystem>().update(registry.get());
	registry->getSystem<CollisionSystem>().update(eventBus.get());
}

void Game::render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// invoke all systems that need to render
	registry->getSystem<RenderSystem>().update(renderer, resources.get(), camera);
	registry->getSystem<RenderTextSystem>().update(renderer, resources.get(), camera);
	registry->getSystem<HealthBarRenderSystem>().update(renderer, resources.get(), camera);
	if(isDebug){
		registry->getSystem<DebugRenderSystem>().update(renderer, camera);

		registry->getSystem<RenderGUISystem>().update(renderer, registry.get());
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
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
