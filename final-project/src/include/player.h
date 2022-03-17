#include "entity.h"
#include <glm/glm.hpp>
#include <framework/window.h>

#ifndef Player_H
#define Player_H

class Player : public Entity {
    glm::mat4 d_modelMatrix{ 1.f };
    int verticalInput = 0;
    int horizontalInput = 0;

    const Window* d_window;
    static constexpr glm::vec3 s_yAxis { 0, 1, 0 };

public:
    Player(Window* window,
           std::string const& name,
           int const& health,
           std::filesystem::path const& texture,
           std::filesystem::path const& mesh,
           glm::vec3 const& position,
           glm::vec3 const& rotation);
   
    void move(float deltaTime);
    void rotate(float deltaTime);
    void updateInput();
    void lookAt(glm::vec3 lookPoint);

    glm::mat4 modelMatrix();
    glm::vec3 position();

    static const int RUN_SPEED  = 10;
    static const int TURN_SPEED = 500;
    static const int GRAVITY    = -50;
    static const int JUMP_POWER = 30;

    int rotationInput = 0;

private:

};

#endif
