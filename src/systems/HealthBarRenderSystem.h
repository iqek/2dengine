#pragma once

#include "../ecs/ECS.h"
#include "../components/TransformComponent.h"
#include "../components/HealthComponent.h"
#include "../components/SpriteComponent.h"

class HealthBarRenderSystem : public System {
public:
    HealthBarRenderSystem() {
        requireComponent<TransformComponent>();
        requireComponent<HealthComponent>();
        requireComponent<SpriteComponent>();
    }

    void update(SDL_Renderer* renderer, ResourceManager* resources, const SDL_FRect& camera) {
        for (auto entity: getSystemEntities()) {
            const auto transform = entity.getComponent<TransformComponent>();
            const auto health = entity.getComponent<HealthComponent>();
            const auto sprite = entity.getComponent<SpriteComponent>();

            SDL_Color color;

            if (health.percentage >= 0 && health.percentage < 40 ) {
                color = {255, 0, 0};
            } else if (health.percentage >= 40 && health.percentage < 80 ) {
                color = {255, 255, 0};
            } else {
                color = {0, 255, 0};
            }

            float spritePosX = transform.position.x - camera.x;
            float spritePosY = transform.position.y - camera.y;
            float spriteCenterX = spritePosX + sprite.width * transform.scale.x / 2.0f;

            // Render the health percentage text
            std::string healthText = std::to_string(health.percentage);
            SDL_Surface* surface = TTF_RenderText_Blended(resources->getFont("pico8-font-5"), healthText.c_str(), healthText.size(), color);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);

            float labelW;
            float labelH;
            SDL_GetTextureSize(texture, &labelW, &labelH);

            // Render the health bar
            float barW = 15;
            float barH = 3;
            float gap = 2;
            float barPosX = spriteCenterX - barW / 2.0f;
            float barPosY = spritePosY - gap - barH;

            SDL_FRect bar = {barPosX, barPosY, barW * (health.percentage / 100.0f), barH};
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
            SDL_RenderFillRect(renderer, &bar);

            SDL_FRect dstRect = {
                spriteCenterX - labelW / 2.0f, barPosY - gap - labelH, labelW, labelH
            };

            SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
        }
    }
};