#include "entity.h"
#include <framework/window.h>
#include "mousepicker.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player : public Entity
{
	Window* const m_window;

	int m_verticalInput{ 0 };
	int m_horizontalInput { 0 };
	bool m_moving{ false };

	glm::mat4 m_projectionMatrix{0.f};
	MousePicker m_picker{m_window, m_projectionMatrix};

	public:
		Player(std::filesystem::path const& path, Window* window, glm::mat4 projectionMatrix);

		void move(float deltaTime);
		void updateInput();
		void lookAt(glm::vec3 const &camPos, glm::mat4 const& camViewMatrix);

		//void draw(Shader& shader);
		//void bindTexture(int slot, int location);

		static const int RUN_SPEED = 10;
		static const int TURN_SPEED = 500;
		static const int GRAVITY = -50;
		static const int JUMP_POWER = 30;
};

#endif