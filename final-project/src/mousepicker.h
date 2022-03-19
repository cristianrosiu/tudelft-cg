#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <framework/window.h>

class MousePicker
{
	glm::vec3 d_currentRayDir {0.f};
	glm::mat4 d_projectionMatrix {1.f};
	glm::vec3 d_eyeRay{ 0.f };

	const Window* d_pWindow;

	public:
		MousePicker(Window *d_pWindow, glm::mat4 const& projectionMatrix);

		glm::vec3 getRayPoint(glm::vec3 const& origin, glm::vec3 const& planePoint, glm::vec3 const& planeNormal);
		
		void update(glm::mat4 const& viewMatrix);
};