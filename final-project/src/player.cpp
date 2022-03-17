#include "include/player.h"
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Player::Player(Window* window,
    std::string const& name, int const& health,
    std::filesystem::path const& texture,
    std::filesystem::path const& mesh,
    glm::vec3 const& position, glm::vec3 const& rotation)
    :
    Entity(),
    d_window(window)
{}

glm::mat4 Player::modelMatrix()
{
    return d_modelMatrix;
}

glm::mat4 Player::rotationMatrix()
{
    return d_rotationMatrix;
}

glm::vec3 Player::position()
{
    return glm::vec3(d_modelMatrix * glm::vec4(d_position, 1.0f));
}

glm::vec3 Player::rotation()
{
    return glm::vec3(d_rotationMatrix * glm::vec4(d_rotation, 1.0f));
}


void Player::move(float deltaTime)
{
    updateInput();
    glm::vec3 velocity = glm::vec3(horizontalInput * RUN_SPEED * deltaTime, 0.f, verticalInput * RUN_SPEED * deltaTime);
    d_modelMatrix = glm::translate(d_modelMatrix, velocity);
    //d_modelMatrix = glm::rotate(d_modelMatrix, glm::radians(horizontalInput * TURN_SPEED * deltaTime), glm::vec3(0.f, 1.f, 0.f));
}

void Player::rotate(float deltaTime /*glm::vec3 lookPoint*/)
{
    updateInput();
    d_modelMatrix = glm::rotate(d_modelMatrix, glm::radians(rotationInput * TURN_SPEED * deltaTime), glm::vec3(0.f, 1.f, 0.f));
    d_rotationMatrix = glm::rotate(d_rotationMatrix, glm::radians(rotationInput * TURN_SPEED * deltaTime), glm::vec3(0.f, 1.f, 0.f));
    d_forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(rotationInput * TURN_SPEED * deltaTime), glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(d_forward, 1.f)));
    
    //d_modelMatrix = d_modelMatrix*glm::lookAt(d_position, glm::vec3(lookPoint.x, d_position.y, lookPoint.z), glm::vec3(0.f, 1.f, 0.f));
    //d_modelMatrix = glm::rotate(d_modelMatrix, glm::radians(horizontalInput * TURN_SPEED * deltaTime), glm::vec3(0.f, 1.f, 0.f));
}

void Player::lookAt(glm::vec3 lookPoint)
{
    updateInput();
    d_modelMatrix = glm::lookAt(d_position, glm::vec3(lookPoint.x, d_position.y, lookPoint.z), glm::vec3(0.f, 1.f, 0.f));
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

    if (d_window->isKeyPressed(GLFW_KEY_E))
        rotationInput = -1;
    else if (d_window->isKeyPressed(GLFW_KEY_Q))
        rotationInput = 1;
    else
        rotationInput = 0;
}

