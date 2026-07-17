#pragma once

#include <SDL3/SDL.h>

struct ProjectileComponent {  // change the whole projectile-health-damage system later
    bool isFriendly;
    int hitDamage;
    int duration;
    int startTime;

    ProjectileComponent(bool isFriendly = false, int hitDamage = 0, int duration = 0)
        : isFriendly(isFriendly), hitDamage(hitDamage), duration(duration), startTime(SDL_GetTicks())
    {}
};
