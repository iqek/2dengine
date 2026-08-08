#include "Game.h"
#include "LevelLoader.h"
#include "../ecs/ECS.h"
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
#include "../systems/ScriptSystem.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>
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

void Game::setup() {
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
	registry->addSystem<ScriptSystem>();

	// Create the bindings between C++ and Lua
	registry->getSystem<ScriptSystem>().createLuaBindings(lua);

	// load the first level
	LevelLoader loader;
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
	loader.loadLevel(lua, registry.get(), resources.get(), renderer, 2);
}

void Game::update(){
	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
	millisecsPreviousFrame = SDL_GetTicks();

	// reset al event handlers for the current frame
	eventBus->reset();
	// perform the subscription of events for all systems
	registry->getSystem<MovementSystem>().listenToEvents(eventBus.get());
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
	registry->getSystem<ScriptSystem>().update(deltaTime, SDL_GetTicks());
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
