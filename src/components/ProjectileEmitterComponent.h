#pragma once

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

struct ProjectileEmitterComponent {
    glm::vec2 projectileVelocity;
    Uint64 repeatFrequency;
    Uint64 projectileDuration;
    int hitDamage;
    bool isFriendly;
    Uint64 lastEmissionTime;

    ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0), Uint64 repeatFrequency = 0, Uint64 projectileDuration = 10000, int hitDamage = 0, bool isFriendly = false)
        : projectileVelocity(projectileVelocity), repeatFrequency(repeatFrequency), projectileDuration(projectileDuration),
          hitDamage(hitDamage), isFriendly(isFriendly), lastEmissionTime(SDL_GetTicks())
    {}
};
