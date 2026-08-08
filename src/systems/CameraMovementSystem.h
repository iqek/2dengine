#pragma once

#include "../ecs/ECS.h"
#include "../components/CameraFollowComponent.h"
#include "../components/TransformComponent.h"
#include <SDL3/SDL.h>
#include <algorithm>

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
            // (if the viewport is bigger than the map on an axis, clamp to 0 instead of a negative offset)
            float maxCameraX = std::max(0.0f, Game::mapWidth - camera.w);
            float maxCameraY = std::max(0.0f, Game::mapHeight - camera.h);
            camera.x = std::clamp(camera.x, 0.0f, maxCameraX);
            camera.y = std::clamp(camera.y, 0.0f, maxCameraY);
        }
    }
};