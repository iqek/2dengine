#pragma once

#include "../ecs/ECS.h"
#include "../components/BoxColliderComponent.h"
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
        spdlog::info("the damage system received an event collision between entities {} {}", std::to_string(event.a.getId()), std::to_string(event.b.getId()));
        //event.a.kill();
        //event.b.kill();
    }

    void update() {

    }
};