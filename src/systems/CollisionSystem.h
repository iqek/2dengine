#pragma once

#include "../ecs/ECS.h"
#include "../components/BoxColliderComponent.h"
#include "../components/TransformComponent.h"
#include <spdlog/spdlog.h>

class CollisionSystem: public System {
public:
    CollisionSystem(){
        requireComponent<BoxColliderComponent>();
        requireComponent<TransformComponent>();
    }

    void update(){
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
                }
            }
        }
    }

    bool checkAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH){
        return (
            aX < bX + bW && 
            aX + aW > bX && 
            aY < bY + bH && 
            aY + aH > bY
        );
    }
};