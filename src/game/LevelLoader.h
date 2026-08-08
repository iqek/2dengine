#pragma once

#include "../ecs/ECS.h"
#include "../resources/ResourceManager.h"
#include "SDL3/SDL.h"
#include <sol/sol.hpp>

class LevelLoader {
public:
    LevelLoader();
    ~LevelLoader();
    void loadLevel(sol::state& lua, Registry* registry, ResourceManager* resources, SDL_Renderer* renderer, int level);
};
