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

    void update(SDL_Renderer* renderer, ResourceManager* resources, SDL_FRect& camera){
        auto entities = getSystemEntities();

        std::stable_sort(entities.begin(), entities.end(), [](const Entity& a, const Entity& b){
            return a.getComponent<SpriteComponent>().zIndex < b.getComponent<SpriteComponent>().zIndex;
        });

        SDL_FRect screenRect = {0.0f, 0.0f, camera.w, camera.h};

        for(auto entity: entities){
            const auto transform = entity.getComponent<TransformComponent>();
            const auto sprite = entity.getComponent<SpriteComponent>();

            // set the source rectangle of the original sprite
            SDL_FRect srcRect = sprite.srcRect;

            // set the destination rectangle with the x,y position to be rendered
            SDL_FRect dstRect = {
                transform.position.x - (sprite.isFixed ? 0.0f : camera.x),
                transform.position.y - (sprite.isFixed ? 0.0f : camera.y),
                sprite.width * transform.scale.x,
                sprite.height * transform.scale.y
            };

            if(!SDL_HasRectIntersectionFloat(&dstRect, &screenRect)) continue;

            SDL_RenderTextureRotated(
                renderer,
                resources->getTexture(sprite.name),
                &srcRect, 
                &dstRect,
                transform.rotation,
                nullptr,
                sprite.flip
            );
        }
    }
};