#pragma once

struct HealthComponent {
    int percentage;

    HealthComponent(int percentage = 0)
        : percentage(percentage)
    {}
};