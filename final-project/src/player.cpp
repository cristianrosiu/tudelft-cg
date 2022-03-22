#include "player.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include "glm/gtx/string_cast.hpp"

Player::Player(std::filesystem::path const& path, Window* window, glm::mat4 projectionMatrix)
    : Entity(path), m_window(window), m_projectionMatrix(projectionMatrix), m_picker(window, projectionMatrix)
{}

void Player::move(float deltaTime)
{
    updateInput();
    glm::vec3 velocity = glm::vec3(m_horizontalInput, 0.f, m_verticalInput) * (float)RUN_SPEED * deltaTime;
    //d_position = glm::translate(glm::mat4(1.f), velocity) * glm::vec4(d_position, 1.f);

    transform.setLocalPosition(transform.getLocalPosition() + velocity);
    updateSelfAndChild();
}

void Player::lookAt(glm::vec3 const &camPos, glm::mat4 const& camViewMatrix)
{
    m_picker.update(camViewMatrix);
    auto lookPoint = m_picker.getRayPoint(camPos, glm::vec3(0.f), glm::normalize(glm::vec3(0.f, 1.f, 0.f)));

    glm::vec3 curentPos = transform.getLocalPosition();
    glm::vec3 targetPos = glm::vec3(lookPoint.x, 0.f, lookPoint.z);

    glm::vec3 viewForward = glm::normalize(targetPos - curentPos);

    float angle = std::atan2f(viewForward.z, viewForward.x);

    //d_rotationMatrix = glm::rotate(glm::mat4(1.f), glm::radians(90.f), d_up) * glm::rotate(glm::mat4(1.f), angle, d_up);
    transform.setLocalRotation(glm::vec3(0.f, glm::degrees(angle) + 90.f, 0.f));
    updateSelfAndChild();
}

void Player::updateInput()
{
    if (m_window->isKeyPressed(GLFW_KEY_W))
        m_verticalInput = 1;
    else if (m_window->isKeyPressed(GLFW_KEY_S))
        m_verticalInput = -1;
    else
        m_verticalInput = 0;

    if (m_window->isKeyPressed(GLFW_KEY_A))
        m_horizontalInput = 1;
    else if (m_window->isKeyPressed(GLFW_KEY_D))
        m_horizontalInput = -1;
    else
        m_horizontalInput = 0;

    m_moving = (m_horizontalInput != 0) || (m_verticalInput != 0);
}
