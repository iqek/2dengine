#pragma once

#include "../ecs/ECS.h"
#include "../components/BoxColliderComponent.h"
#include "../components/TransformComponent.h"
#include "../event_manager/EventBus.h"
#include "../events/CollisionEvent.h"

class CollisionSystem: public System {
public:
    CollisionSystem(){
        requireComponent<BoxColliderComponent>();
        requireComponent<TransformComponent>();
    }

    void update(EventBus* eventBus) {
        auto entities = getSystemEntities();

        for(auto entity: entities){
            entity.getComponent<BoxColliderComponent>().isColliding = false;
        }

        for(auto i = entities.begin(); i != entities.end(); i++){
            Entity a = *i;
            auto aTransform = a.getComponent<TransformComponent>();
            auto& aCollider = a.getComponent<BoxColliderComponent>();

            for(auto j = i; j != entities.end(); j++){
                Entity b = *j;

                if(a == b) continue;

                auto bTransform = b.getComponent<TransformComponent>();
                auto& bCollider = b.getComponent<BoxColliderComponent>();

                // check for collision between a and b
                bool collisionHappened = checkAABBCollision(
                    aTransform.position.x + aCollider.offset.x, aTransform.position.y + aCollider.offset.y,
                    aCollider.width, aCollider.height,
                    bTransform.position.x + bCollider.offset.x, bTransform.position.y + bCollider.offset.y,
                    bCollider.width, bCollider.height
                );

                if(collisionHappened){
                    spdlog::info("Entity {} is colliding with {}", std::to_string(a.getId()), std::to_string(b.getId()));
                    aCollider.isColliding = true;
                    bCollider.isColliding = true;

                    // emit an event ...
                    eventBus->emitEvent<CollisionEvent>(a, b);
                }
            }
        }
    }

    bool checkAABBCollision(float aX, float aY, float aW, float aH, float bX, float bY, float bW, float bH){
        return (
            aX < bX + bW && 
            aX + aW > bX && 
            aY < bY + bH && 
            aY + aH > bY
        );
    }
};