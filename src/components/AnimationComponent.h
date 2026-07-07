#pragma once

#include <SDL3/SDL.h>

struct AnimationComponent{
    int frameNum;
    int currFrame;
    int frameSpeed;
    bool isLoop;
    int startTime;
    
    AnimationComponent(int frameNum = 1, int frameSpeed = 1, bool isLoop = true){
        this->frameNum = frameNum;
        this->currFrame = 1;
        this->frameSpeed = frameSpeed;
        this->isLoop = isLoop;
        this->startTime = SDL_GetTicks();
    }
};