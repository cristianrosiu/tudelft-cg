#include "camera.h"
// Suppress warnings in third-party code.
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
DISABLE_WARNINGS_POP()

#include <iostream>

PlayerCamera::PlayerCamera(Window* window, Player* target)
    : m_window(window), m_target(target)
{}

glm::mat4 PlayerCamera::viewMatrix()
{
    return glm::lookAt(transform.getGlobalPosition(), m_target->transform.getLocalPosition(), transform.getUp());
}

void PlayerCamera::update()
{

    transform.setLocalPosition(m_target->transform.getLocalPosition() + glm::vec3(0.f, 4.f, -4.f));
    updateSelfAndChild();

    

}
