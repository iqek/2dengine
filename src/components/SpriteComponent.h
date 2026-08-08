#pragma once

#include <string>
#include <SDL3/SDL.h>

struct SpriteComponent{
    std::string name;
    int width;
    int height;
    int zIndex;  // bunu daha sonra layer enumı ile yap (tilemap, enemies etc.)
    SDL_FlipMode flip;
    bool isFixed;
    SDL_FRect srcRect;

    SpriteComponent(std::string name = "", int width = 0, int height = 0, int zIndex = 0, bool isFixed = false, float srcRectX = 0.0f, float srcRectY = 0.0f)
        : name(std::move(name)), width(width), height(height), zIndex(zIndex), flip(SDL_FLIP_NONE), isFixed(isFixed),
          srcRect{srcRectX, srcRectY, static_cast<float>(width), static_cast<float>(height)}
    {}
};