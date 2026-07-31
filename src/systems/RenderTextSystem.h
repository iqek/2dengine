#pragma once

#include "../ecs/ECS.h"
#include "../resources/ResourceManager.h"
#include "../components/TransformComponent.h"
#include "../components/TextLabelComponent.h"
#include <SDL3/SDL.h>

class RenderTextSystem: public System {
public:
    RenderTextSystem() {
        requireComponent<TransformComponent>();
        requireComponent<TextLabelComponent>();
    }

    void update(SDL_Renderer* renderer, ResourceManager* resources, const SDL_FRect& camera) {
        for (auto entity: getSystemEntities()) {
            auto transform = entity.getComponent<TransformComponent>();
            const auto textLabel = entity.getComponent<TextLabelComponent>();

            SDL_Surface* surface = TTF_RenderText_Blended(resources->getFont(textLabel.font), textLabel.text.c_str(), textLabel.text.size(), textLabel.color);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);

            float width;
            float height;
            SDL_GetTextureSize(texture, &width, &height);

            SDL_FRect dstRect = {
                transform.position.x - (textLabel.isFixed ? 0.0f : camera.x),
                transform.position.y - (textLabel.isFixed ? 0.0f : camera.y),
                width,
                height
            };

            SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
        }
    }
};