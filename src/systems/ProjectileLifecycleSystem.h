#pragma once

#include "../ecs/ECS.h"
#include "../components/ProjectileComponent.h"

class ProjectileLifecycleSystem: public System {
public:
    ProjectileLifecycleSystem() {
        requireComponent<ProjectileComponent>();
    }

    void update() {
        for (auto entity: getSystemEntities()) {
            auto projectile = entity.getComponent<ProjectileComponent>();

            // Kill projectiles after they reach their duration limit
            if (SDL_GetTicks() - projectile.startTime > projectile.duration) {
                entity.kill();
            }
        }
    }
};