#include <glm/glm.hpp>

#ifndef UTILITY_H
#define UTILITY_H

static struct Light {
    glm::vec3 position;
    glm::mat4 viewMatrix;
    glm::vec3 color;
};


#endif