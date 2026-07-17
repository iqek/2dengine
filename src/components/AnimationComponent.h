#pragma once

#include <SDL3/SDL.h>

struct AnimationComponent{
    int frameNum;
    int currFrame;
    int frameSpeed;
    bool isLoop;
    int startTime;
    
    AnimationComponent(int frameNum = 1, int frameSpeed = 1, bool isLoop = true)
        : frameNum(frameNum), currFrame(1), frameSpeed(frameSpeed), isLoop(isLoop), startTime(SDL_GetTicks())
    {}
};