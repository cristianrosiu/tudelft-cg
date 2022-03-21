#include "entity.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <framework/window.h>

#ifndef Player_H
#define Player_H

class Player : public Entity {
    glm::mat4 d_modelMatrix{ 1.f };
    glm::mat4 d_transMatrix{ 1.f };
    glm::mat4 d_rotationMatrix{ 1.f };
    int verticalInput = 0;
    int horizontalInput = 0;


    glm::dvec2 currentMousePos{ 0.f };
    glm::dvec2 prevMousePos{ 0.f };
    glm::dvec2 mouseInput{ 0.f };

    Window* const d_window;
    static constexpr glm::vec3 s_yAxis { 0, 1, 0 };

public:
    Player(Window* window,
           std::string const& name,
           int const& health,
           std::filesystem::path const& texture,
           std::filesystem::path const& sTexture,
           std::filesystem::path const& mesh,
           glm::vec3 const& position,
           glm::vec3 const& rotation);
   
    void move(float deltaTime);
    void updateInput();
    void lookAt(glm::vec3 const& lookPoint);

    glm::mat4 const modelMatrix() const;
    glm::vec3 const position() const;

    static const int RUN_SPEED  = 10;
    static const int TURN_SPEED = 500;
    static const int GRAVITY    = -50;
    static const int JUMP_POWER = 30;
    bool isMoving{ false };

    int rotationInput = 0;

private:

};

#endif
