#pragma once

struct HealthComponent {
    int health;

    HealthComponent(int health = 0)
        : health(health)
    {}
};