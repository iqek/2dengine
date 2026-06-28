#pragma once

#include "../ecs/ECS.h"
#include "../components/TransformComponent.h"
#include "../components/SpriteComponent.h"
#include <SDL3/SDL.h>

class RenderSystem: public System{
public:
    RenderSystem(){
        requireComponent<TransformComponent>();
        requireComponent<SpriteComponent>();
    }

    void update(SDL_Renderer* renderer){
        for(auto entity: getSystemEntities()){
            const auto transform = entity.getComponent<TransformComponent>();
            const auto sprite = entity.getComponent<SpriteComponent>();

            SDL_FRect objRect = {
                static_cast<float>(transform.position.x), static_cast<float>(transform.position.y),
                static_cast<float>(sprite.width), static_cast<float>(sprite.height)
            };

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &objRect);
        }
    }
};