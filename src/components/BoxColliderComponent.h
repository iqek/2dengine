#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent {  // add pixel collider later
    float width;
    float height;
    glm::vec2 offset;
    bool isColliding;

    BoxColliderComponent(float width = 0, float height = 0, glm::vec2 offset = glm::vec2(0))
        : width(width), height(height), offset(offset), isColliding(false)
    {}
};
