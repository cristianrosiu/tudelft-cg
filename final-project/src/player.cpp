#include "player.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include "glm/gtx/string_cast.hpp"

Player::Player(std::filesystem::path const& path, Window* window, glm::mat4 projectionMatrix)
    : GameObject(path), m_window(window), m_projectionMatrix(projectionMatrix), m_picker(window, projectionMatrix)
{}

void Player::update(glm::vec3 const& camPos, glm::mat4 const& camViewMatrix, float const &deltaTime)
{
    updateInput();
    move(deltaTime);
    lookAt(camPos, camViewMatrix);
    updateSelfAndChild();

    if (m_moving)
    {
        if (m_currentFrame == m_meshes.size() - 1)
            m_currentFrame = 0;
        m_currentFrame++;
    }
    else
        m_currentFrame = 0;
}

void Player::move(float deltaTime)
{
    glm::vec3 velocity = glm::vec3(m_horizontalInput, 0.f, m_verticalInput) * (float)RUN_SPEED * deltaTime;
    transform.setLocalPosition(transform.getLocalPosition() + velocity);
}

void Player::lookAt(glm::vec3 const &camPos, glm::mat4 const& camViewMatrix)
{
    m_picker.update(camViewMatrix);
    auto lookPoint = m_picker.getRayPoint(camPos, glm::vec3(0.f), glm::normalize(glm::vec3(0.f, 1.f, 0.f)));

    glm::vec3 curentPos = transform.getLocalPosition();
    glm::vec3 targetPos = glm::vec3(lookPoint.x, 0.f, lookPoint.z);

    glm::vec3 viewForward = glm::normalize(targetPos - curentPos);

    float angle = std::atan2f(viewForward.z, viewForward.x);

    transform.setLocalRotation(glm::vec3(0.f, glm::degrees(angle) + 90.f, 0.f));
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

#include <iostream>
void Player::draw(Shader& shader)
{
    m_meshes[m_currentFrame].draw(shader);
}