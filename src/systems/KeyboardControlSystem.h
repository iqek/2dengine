#pragma once

#include "../ecs/ECS.h"
#include "../event_manager/EventBus.h"
#include "../events/KeyPressedEvent.h"

class KeyboardControlSystem: public System {
public:
    KeyboardControlSystem() {}

    void listenToEvents(EventBus* eventBus) {
        eventBus->listenToEvent(this, &KeyboardControlSystem::onKeyPressed);
    }

    void onKeyPressed(KeyPressedEvent& event) {
        std::string keyCode = std::to_string(event.symbol);
        std::string keySymbol(1, event.symbol);
        spdlog::info("Key pressed event emitted: [ {} ] {}", keyCode, keySymbol);
    }

    void update() {}
};