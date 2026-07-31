#pragma once

#include "../ecs/ECS.h"
#include "../components/AnimationComponent.h"
#include "../components/SpriteComponent.h"
#include <SDL3/SDL.h>

class AnimationSystem: public System {
public: 
	AnimationSystem(){
        requireComponent<AnimationComponent>();
        requireComponent<SpriteComponent>();
	}

	void update(){
		for(auto entity: getSystemEntities()){
			auto& animation = entity.getComponent<AnimationComponent>();
            auto& sprite = entity.getComponent<SpriteComponent>();

            animation.currFrame = static_cast<int>((SDL_GetTicks() - animation.startTime) * animation.frameSpeed / 1000 % animation.frameNum);
            sprite.srcRect.x = static_cast<float>(animation.currFrame * sprite.width);
        }
	}
};