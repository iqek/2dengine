#pragma once

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

struct ProjectileEmitterComponent {
    glm::vec2 projectileVelocity;
    int repeatFrequency;
    int projectileDuration;
    int hitDamage;
    bool isFriendly;
    int lastEmissionTime;

    ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0), int repeatFrequency = 0, int projectileDuration = 10000, int hitDamage = 0, bool isFriendly = false)
        : projectileVelocity(projectileVelocity), repeatFrequency(repeatFrequency), projectileDuration(projectileDuration),
          hitDamage(hitDamage), isFriendly(isFriendly), lastEmissionTime(SDL_GetTicks())
    {}
};
