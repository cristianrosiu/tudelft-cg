// Suppress warnings in third-party code.
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
DISABLE_WARNINGS_POP()
#include <framework/window.h>
#include "gameobject.h";
#include "player.h"

#ifndef PLAYERCAMERA_H
#define PLAYERCAMERA_H

class PlayerCamera : public GameObject {
public:
    PlayerCamera(Window* window, Player* target);

    void update();
    glm::mat4 viewMatrix();

private:
    const Window* m_window;
    Player* m_target;
};

#endif