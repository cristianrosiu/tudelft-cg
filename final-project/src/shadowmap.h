#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <framework/shader.h>
#include <initializer_list>
#include "utility.h"
#include "gameobject.h"

#ifndef SHADOWMAP_H
#define SHADOWMAP_H

class ShadowMap
{
	GLuint m_texShadow;
	GLuint m_framebuffer;
	glm::uvec2 m_size;

	public:
		ShadowMap(glm::uvec2 const &size);

		void renderShadowMap(Shader& shader, glm::mat4 const& projectionMatrix, Light light, GameObject &gameObjects,...);
		void bind(int slot, int location);
		GLuint const getTextureID() const;
};

#endif