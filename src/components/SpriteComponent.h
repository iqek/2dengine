#pragma once

#include <string>
#include <SDL3/SDL.h>

struct SpriteComponent{
    std::string name;
    int width;
    int height;
    int zIndex;  // bunu daha sonra layer enumı ile yap (tilemap, enemies etc.)
    SDL_FRect srcRect;

    SpriteComponent(std::string name = "", int width = 0, int height = 0, int zIndex = 0, double srcRectX = 0, double srcRectY = 0){
        this->name = name;
        this->width = width;
        this->height = height;
        this->zIndex = zIndex;
        this->srcRect = {static_cast<float>(srcRectX), static_cast<float>(srcRectY), static_cast<float>(width), static_cast<float>(height)};
    }
};