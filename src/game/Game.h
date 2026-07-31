#pragma once

#include <SDL3/SDL.h>
#include "../ecs/ECS.h"
#include "../resources/ResourceManager.h"
#include "../event_manager/EventBus.h"

constexpr int FPS = 60;
constexpr int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
private:
	Uint64 millisecsPreviousFrame = 0;
	bool isRunning;
	bool isDebug;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_FRect camera;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<ResourceManager> resources;
	std::unique_ptr<EventBus> eventBus;

public:
	Game();
	~Game();
	void initialize();
	void run();
	void loadLevel(int level);
	void setup();
	void processInput();
	void update();
	void render();
	void destroy();

	static int windowWidth;
	static int windowHeight;
	static int mapWidth;
	static int mapHeight;
};
