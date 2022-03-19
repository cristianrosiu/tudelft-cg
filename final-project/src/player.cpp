#include "include/player.h"
#include <iostream>

#define PI 3.14159265358979323846264338327950288
Player::Player(Window* window,
    std::string const& name, int const& health,
    std::filesystem::path const& texture,
    std::filesystem::path const& mesh,
    glm::vec3 const& position, glm::vec3 const& rotation)
    :
    Entity(name, health, texture, mesh, position, rotation),
    d_window(window)
{}

glm::mat4 const Player::modelMatrix() const
{
    return d_transMatrix*d_rotationMatrix;
}

glm::vec3 const Player::position() const
{
    return d_position;
}

void Player::move(float deltaTime)
{
    updateInput();
    glm::vec3 velocity = glm::vec3(horizontalInput, 0.f, verticalInput) * (float)RUN_SPEED * deltaTime;
    d_transMatrix = glm::translate(d_transMatrix, velocity);
    d_position = glm::translate(glm::mat4(1.f), velocity) * glm::vec4(d_position, 1.f);
}

void Player::lookAt(glm::vec3 const& lookPoint)
{
    glm::vec3 curentPos = position();
    glm::vec3 targetPos = glm::vec3(lookPoint.x, 0.f, lookPoint.z);

    glm::vec3 viewForward = glm::normalize(targetPos - curentPos);

    float cosa = glm::dot(d_forward, viewForward);
    float angle = std::atan2f(viewForward.z, viewForward.x);

    d_rotationMatrix = glm::rotate(glm::mat4(1.f), glm::radians(90.f), d_up) * glm::rotate(glm::mat4(1.f), angle, d_up);
    d_forward = glm::vec3(glm::normalize(d_rotationMatrix * glm::vec4(d_forward, 1.f)));
}

void Player::updateInput()
{
    if (d_window->isKeyPressed(GLFW_KEY_W))
        verticalInput = -1;
    else if (d_window->isKeyPressed(GLFW_KEY_S))
        verticalInput = 1;
    else
        verticalInput = 0;
    
    if (d_window->isKeyPressed(GLFW_KEY_A))
        horizontalInput = -1;
    else if (d_window->isKeyPressed(GLFW_KEY_D))
        horizontalInput = 1;
    else
        horizontalInput = 0;
    
    isMoving = (horizontalInput != 0) || (verticalInput != 0);
}

