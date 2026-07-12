#pragma once

#include "../ecs/ECS.h"
#include "../event_manager/Event.h"
#include <SDL3/SDL.h>

class KeyPressedEvent: public Event {
public:
    SDL_Keycode symbol;
    KeyPressedEvent(SDL_Keycode symbol): symbol(symbol) {};
};