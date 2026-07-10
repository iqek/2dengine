#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent {  // add pixel collider later
    double width;
    double height;
    glm::vec2 offset;
    bool isColliding;

    BoxColliderComponent(double width = 0, double height = 0, glm::vec2 offset = glm::vec2(0)){
        this->width = width;
        this->height = height;
        this->offset = offset;
        this->isColliding = false;
    }
};
