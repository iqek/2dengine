#pragma once

#include "../ecs/ECS.h"
#include "../components/BoxColliderComponent.h"
#include "../components/ProjectileComponent.h"
#include "../components/HealthComponent.h"
#include "../event_manager/EventBus.h"
#include "../events/CollisionEvent.h"

class DamageSystem: public System {
public:
    DamageSystem() {
        requireComponent<BoxColliderComponent>();
    }

    void listenToEvents(EventBus* eventBus) {
        eventBus->listenToEvent(this, &DamageSystem::onCollision);
    }

    void onCollision(CollisionEvent& event) {
        Entity a = event.a;
        Entity b = event.b;
        spdlog::info("the damage system received an event collision between entities {} {}", std::to_string(a.getId()), std::to_string(b.getId()));

        if (a.isInGroup("projectiles") && (b.hasTag("player") || b.isInGroup("enemies"))) {
            onProjectileHitsEntity(a, b); // a is the projectile and b is the player
        }

        if (b.isInGroup("projectiles") && (a.hasTag("player") || a.isInGroup("enemies"))) {
            onProjectileHitsEntity(b, a);
        }
    }

    void onProjectileHitsEntity(Entity projectile, Entity entity) {

        bool isPlayer = entity.hasTag("player");
        bool isEnemy = entity.isInGroup("enemies");

        if (!isPlayer && !isEnemy) return;

        auto projectileComponent = projectile.getComponent<ProjectileComponent>();

        // Friendly (player) projectiles damage enemies; unfriendly (enemy) projectiles damage the player
        bool shouldDamage = (projectileComponent.isFriendly && isEnemy) || (!projectileComponent.isFriendly && isPlayer);

        if (shouldDamage) {
            auto& health = entity.getComponent<HealthComponent>();

            health.percentage -= projectileComponent.hitDamage;

            if (health.percentage <= 0) {
                entity.kill();
            }

            projectile.kill();
        }
    }

    void update() {}
};