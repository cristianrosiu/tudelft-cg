#include "entity.h"
#include <glm/glm.hpp>
#include <framework/window.h>

class Player : public Entity {
    static const int RUN_SPEED  = 10;
    static const int TURN_SPEED = 500;
    static const int GRAVITY    = -50;
    static const int JUMP_POWER = 30;

    glm::mat4 d_modelMatrix{ 1.f };
    int verticalInput = 0;
    int horizontalInput = 0;

    const Window* d_window;

public:
    Player(Window* window,
           std::string const& name,
           int const& health,
           std::filesystem::path const& texture,
           std::filesystem::path const& mesh,
           glm::vec3 const& position,
           glm::vec3 const& rotation);
   
    void move(float deltaTime);
    void updateInput();

    glm::mat4 modelMatrix();
    glm::vec3 position();
    glm::vec3 rotation();

private:

};
