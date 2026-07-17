#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent {  // add pixel collider later
    double width;
    double height;
    glm::vec2 offset;
    bool isColliding;

    BoxColliderComponent(double width = 0, double height = 0, glm::vec2 offset = glm::vec2(0))
        : width(width), height(height), offset(offset), isColliding(false)
    {}
};
