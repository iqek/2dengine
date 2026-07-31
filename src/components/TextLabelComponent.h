#pragma once

#include <string>
#include <SDL3/SDL.h>

struct TextLabelComponent {
    std::string text;
    std::string font;
    SDL_Color color;
    bool isFixed;

    TextLabelComponent(std::string text = "", std::string font = "", const SDL_Color color = {0,0,0,0}, bool isFixed = true)
        : text(text), font(font), color(color), isFixed(isFixed)
    {}
};