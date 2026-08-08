#pragma once

#include "../ecs/ECS.h"
#include "../components/TransformComponent.h"
#include "../components/RigidbodyComponent.h"
#include "../components/SpriteComponent.h"
#include "../components/BoxColliderComponent.h"
#include "../components/HealthComponent.h"
#include "../components/ProjectileEmitterComponent.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>

// Add logger gui later

class RenderGUISystem : public System {
public:
    RenderGUISystem() = default;

    void update(SDL_Renderer* renderer, Registry* registry) {
        // draw all the ImGui objects on the screen
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if(ImGui::Begin("Spawn Enemies")) {
            // Input for the enemy X and Y positions
            static int xPos = 0;
            static int yPos = 0;
            static int xScale = 0;
            static int yScale = 0;
            static int xVel = 0;
            static int yVel = 0;
            static float rotation = 0;
            static float projAngle = 0;
            static int projSpeed = 0;
            static int projFreq = 0;
            static int projDuration = 0;
            static int projDamage = 0;
            static int health = 100;
            const char* sprites[] = {"tank-image", "truck-image"};
            static int spriteIdx = 0;

            // Section to input sprite texture key
            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Combo("texture key", &spriteIdx, sprites, IM_ARRAYSIZE(sprites));
            }
            ImGui::Spacing();

            // Section to input transform values
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("x position", &xPos);
                ImGui::InputInt("y position", &yPos);
                ImGui::SliderInt("x scale", &xScale, 1, 10);
                ImGui::SliderInt("y scale", &yScale, 1 , 10);
                ImGui::SliderAngle("rotation degree", &rotation, 0, 360);
            }
            ImGui::Spacing();

            // Section to input rigidbody values
            if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("x velocity", &xVel);
                ImGui::InputInt("y velocity", &yVel);
            }
            ImGui::Spacing();

            // Section to input projectile values
            if (ImGui::CollapsingHeader("Projectiles", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderAngle("angle", &projAngle, 0, 360);
                ImGui::SliderInt("speed", &projSpeed, 10, 500);
                ImGui::InputInt("frequency", &projFreq);
                ImGui::InputInt("duration", &projDuration);
                ImGui::InputInt("damage", &projDamage);
            }
            ImGui::Spacing();

            // Section to input health
            if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("% health", &health, 0 ,100);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Button to create a new enemy entity
            if(ImGui::Button("Create new enemy")) {
                Entity enemy = registry->createEntity();
                enemy.group("enemies");
                enemy.addComponent<TransformComponent>(glm::vec2(xPos, yPos), glm::vec2(xScale, yScale), rotation);
                enemy.addComponent<RigidbodyComponent>(glm::vec2(xVel, yVel));
                enemy.addComponent<SpriteComponent>(sprites[spriteIdx], 32, 32, 2);
                enemy.addComponent<BoxColliderComponent>(32, 32);
                double projVelX = cos(projAngle) * projSpeed;
                double projVelY = sin(projAngle) * projSpeed;
                enemy.addComponent<ProjectileEmitterComponent>(glm::vec2(projVelX, projVelY), projFreq * 1000, projDuration * 1000, projDamage, false);
                enemy.addComponent<HealthComponent>(health);
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }
};