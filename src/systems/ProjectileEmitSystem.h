#pragma once

#include "../ecs/ECS.h"
#include "../event_manager/EventBus.h"
#include "../events/KeyPressedEvent.h"
#include "../components/ProjectileEmitterComponent.h"
#include "../components/TransformComponent.h"
#include "../components/RigidbodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../components/ProjectileComponent.h"
#include <SDL3/SDL.h>

class ProjectileEmitSystem : public System {
public:
    ProjectileEmitSystem() {
        requireComponent<ProjectileEmitterComponent>();
        requireComponent<TransformComponent>();
    }

    void listenToEvents(EventBus* eventBus) {
        // subscribe to the event of the Space key being pressed
        eventBus->listenToEvent(this, &ProjectileEmitSystem::onKeyPressed);
    }

    void onKeyPressed(KeyPressedEvent& event) {
        if (event.symbol == SDLK_SPACE) {
            spdlog::info("Space pressed");
            for (auto entity: getSystemEntities()) {
                if (entity.hasComponent<CameraFollowComponent>()) {
                    const auto projectileEmitter = entity.getComponent<ProjectileEmitterComponent>();
                    const auto transform = entity.getComponent<TransformComponent>();
                    const auto rigidbody = entity.getComponent<RigidbodyComponent>();

                    // If parent entity has sprite, start the projectile position from the center
                    glm::vec2 projectilePosition = transform.position;
                    if (entity.hasComponent<SpriteComponent>()) {
                        auto sprite = entity.getComponent<SpriteComponent>();
                        projectilePosition.x += (transform.scale.x * sprite.width / 2);
                        projectilePosition.y += (transform.scale.y * sprite.height / 2);
                    }

                    // If parent entity direction is controlled by the keyboard keys, modify the direction of the projectile
                    glm::vec2 projectileVelocity = projectileEmitter.projectileVelocity;
                    int directionX = 0;
                    int directionY = 0;
                    if (rigidbody.velocity.x > 0) directionX = +1;
                    if (rigidbody.velocity.x < 0) directionX = -1;
                    if (rigidbody.velocity.y > 0) directionY = +1;
                    if (rigidbody.velocity.y < 0) directionY = -1;
                    projectileVelocity.x = projectileEmitter.projectileVelocity.x * directionX;
                    projectileVelocity.y = projectileEmitter.projectileVelocity.x * directionY;

                    // create new projectile entity
                    Entity projectile = entity.registry->createEntity();
                    projectile.addComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                    projectile.addComponent<RigidbodyComponent>(projectileVelocity);
                    projectile.addComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                    projectile.addComponent<BoxColliderComponent>(4, 4);
                    projectile.addComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitDamage, projectileEmitter.projectileDuration);
                }
            }
        }
    }

    void update(Registry* registry) {
        for (auto entity : getSystemEntities()) {
            auto& projectileEmitter = entity.getComponent<ProjectileEmitterComponent>();
            const auto transform = entity.getComponent<TransformComponent>();

            if (projectileEmitter.repeatFrequency == 0) {
                continue;
            }

            // Check if it's time to emit a new projectile
            if (SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency) {
                glm::vec2 projectilePosition = transform.position;
                if (entity.hasComponent<SpriteComponent>()) {
                    const auto sprite = entity.getComponent<SpriteComponent>();
                    projectilePosition.x += (transform.scale.x * sprite.width / 2);
                    projectilePosition.y += (transform.scale.y * sprite.height / 2);
                }

                // Add a new projectile entity to the registry
                Entity projectile = registry->createEntity();
                projectile.addComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
                projectile.addComponent<RigidbodyComponent>(projectileEmitter.projectileVelocity);
                projectile.addComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                projectile.addComponent<BoxColliderComponent>(4, 4);
                projectile.addComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitDamage, projectileEmitter.projectileDuration);

                // update the projectile emitters last emission to the current milliseconds
                projectileEmitter.lastEmissionTime = SDL_GetTicks();
            }
        }
    }
};
