#pragma once

#include "../ecs/ECS.h"
#include "../components/TransformComponent.h"
#include "../components/SpriteComponent.h"
#include "../resources/ResourceManager.h"
#include <SDL3/SDL.h>
#include <algorithm>

class RenderSystem: public System{
public:
    RenderSystem(){
        requireComponent<TransformComponent>();
        requireComponent<SpriteComponent>();
    }

    void update(SDL_Renderer* renderer, ResourceManager& resources){
        auto entities = getSystemEntities();

        std::stable_sort(entities.begin(), entities.end(), [](const Entity& a, const Entity& b){
            return a.getComponent<SpriteComponent>().zIndex < b.getComponent<SpriteComponent>().zIndex;
        });

        for(auto entity: entities){
            const auto transform = entity.getComponent<TransformComponent>();
            const auto sprite = entity.getComponent<SpriteComponent>();

            // set the source rectangle of the original sprite
            SDL_FRect srcRect = sprite.srcRect;

            // set the destination rectangle with the x,y position to be rendered
            SDL_FRect dstRect = {
                static_cast<float>(transform.position.x), 
                static_cast<float>(transform.position.y),
                static_cast<float>(sprite.width * transform.scale.x), 
                static_cast<float>(sprite.height * transform.scale.y)
            };

            SDL_RenderTextureRotated(
                renderer, 
                resources.getTexture(sprite.name),
                &srcRect, 
                &dstRect,
                transform.rotation,
                nullptr,
                SDL_FLIP_NONE
            );
        }
    }
};