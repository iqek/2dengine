#pragma once

#include "../ecs/ECS.h"
#include "../event_manager/EventBus.h"
#include "../events/KeyPressedEvent.h"
#include "../components/KeyboardControlledComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/RigidbodyComponent.h"

class KeyboardControlSystem: public System {
public:
    KeyboardControlSystem() {
        requireComponent<KeyboardControlledComponent>();
        requireComponent<SpriteComponent>();
        requireComponent<RigidbodyComponent>();
    }

    void listenToEvents(EventBus* eventBus) {
        eventBus->listenToEvent(this, &KeyboardControlSystem::onKeyPressed);
    }

    void onKeyPressed(KeyPressedEvent& event) {
        // Change the sprite and the velocity of the entity
        for(auto entity: getSystemEntities()) {
            const auto keyboardControl = entity.getComponent<KeyboardControlledComponent>();
            auto& sprite = entity.getComponent<SpriteComponent>();
            auto& rigidbody = entity.getComponent<RigidbodyComponent>();
            
            switch(event.symbol){
                case SDLK_UP:
                    rigidbody.velocity = keyboardControl.upVelocity;
                    sprite.srcRect.y = sprite.height * 0;
                    break;
                case SDLK_RIGHT:
                    rigidbody.velocity = keyboardControl.rightVelocity;
                    sprite.srcRect.y = sprite.height * 1;
                    break;
                case SDLK_DOWN:
                    rigidbody.velocity = keyboardControl.downVelocity;
                    sprite.srcRect.y = sprite.height * 2;
                    break;
                case SDLK_LEFT:
                    rigidbody.velocity = keyboardControl.leftVelocity;
                    sprite.srcRect.y = sprite.height * 3;
                    break;
            }
        }
    }

    void update() {}
};