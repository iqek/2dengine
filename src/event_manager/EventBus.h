#pragma once

#include <spdlog/spdlog.h>
#include <map>
#include <list>
#include <functional>
#include <typeindex>
#include "Event.h"

using HandlerList = std::list<std::function<void(Event&)>>;

class EventBus {
private:
    std::map<std::type_index, std::unique_ptr<HandlerList>> listeners;

public:
    EventBus() {
        spdlog::info("EventBus constructor called");
    }

    ~EventBus() {
        spdlog::info("EventBus destructor called");
    }

    // Clears the subscribers list
    void reset() {
        listeners.clear();
    }

    // Subscribe to an event type <T>
    // a listener subscribes to an event
    // eventBus->listenToEvent<CollisionEvent>(this, &Game::onCollision);
    template <typename TOwner, typename TEvent>
    void listenToEvent(TOwner* ownerInstance, void (TOwner::*callbackFunction)(TEvent&)) {
        if (!listeners[typeid(TEvent)]) {
            listeners[typeid(TEvent)] = std::make_unique<HandlerList>();
        }
        listeners[typeid(TEvent)]->push_back(
            [ownerInstance, callbackFunction](Event& e) {
                std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
            }
        );
    }

    // Emit an event of type <T>
    // as soon as something emits an event all the listener callback functions get executed
    // eventBus->emitEvent<CollisionEvent>(player, enemy)
    template <typename TEvent, typename ...TArgs>
    void emitEvent(TArgs&& ...args) {
        TEvent event(std::forward<TArgs>(args)...);
        auto handlers = listeners[typeid(TEvent)].get();
        if (handlers) {
            for (auto& handler : *handlers) {
                handler(event);
            }
        }
    }
};
