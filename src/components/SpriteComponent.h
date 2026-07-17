#pragma once

#include <string>
#include <SDL3/SDL.h>

struct SpriteComponent{
    std::string name;
    int width;
    int height;
    int zIndex;  // bunu daha sonra layer enumı ile yap (tilemap, enemies etc.)
    bool isFixed;
    SDL_FRect srcRect;

    SpriteComponent(std::string name = "", int width = 0, int height = 0, int zIndex = 0, bool isFixed = false, double srcRectX = 0, double srcRectY = 0)
        : name(std::move(name)), width(width), height(height), zIndex(zIndex), isFixed(isFixed),
          srcRect{static_cast<float>(srcRectX), static_cast<float>(srcRectY), static_cast<float>(width), static_cast<float>(height)}
    {}
};