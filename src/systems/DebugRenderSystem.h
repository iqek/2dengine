#pragma once

#include "../ecs/ECS.h"
#include "../components/TransformComponent.h"
#include "../components/BoxColliderComponent.h"
#include <SDL3/SDL.h>

class DebugRenderSystem: public System {
public:
    DebugRenderSystem(){
        requireComponent<TransformComponent>();
        requireComponent<BoxColliderComponent>();
    }

    void update(SDL_Renderer* renderer, SDL_FRect& camera) {
        for(auto entity: getSystemEntities()){
            const auto transform = entity.getComponent<TransformComponent>();
            const auto collider = entity.getComponent<BoxColliderComponent>();

            SDL_FRect colliderRect = {
                transform.position.x + collider.offset.x - camera.x,
                transform.position.y + collider.offset.y - camera.y,
                collider.width * transform.scale.x,
                collider.height * transform.scale.y
            };
            if(collider.isColliding){
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            }
            SDL_RenderRect(renderer, &colliderRect);
        }
    }
};