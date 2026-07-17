#pragma once

#include "../ecs/ECS.h"
#include "../components/CameraFollowComponent.h"
#include "../components/TransformComponent.h"
#include <SDL3/SDL.h>

class CameraMovementSystem: public System {
public:
    CameraMovementSystem() {
        requireComponent<CameraFollowComponent>();
        requireComponent<TransformComponent>();
    }

    void update(SDL_FRect& camera) {
        for(auto entity: getSystemEntities()) {
            auto transform = entity.getComponent<TransformComponent>();

            if(transform.position.x + (camera.w / 2) < Game::mapWidth ) {
                camera.x = transform.position.x - (Game::windowWidth / 2);
            }
            if(transform.position.y + (camera.h / 2) < Game::mapHeight) {
                camera.y = transform.position.y - (Game::windowHeight / 2);
            }

            // Keep the camera rectangle view inside the map limits
            camera.x = camera.x < 0 ? 0 : camera.x;
            camera.y = camera.y < 0 ? 0 : camera.y;
            camera.x = camera.x > Game::mapWidth - camera.w ? Game::mapWidth - camera.w : camera.x;
            camera.y = camera.y > Game::mapHeight - camera.h ? Game::mapHeight - camera.h : camera.y;
        }
    }
};