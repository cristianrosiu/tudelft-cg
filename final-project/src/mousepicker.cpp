#include "mousepicker.h"
#include <iostream>

MousePicker::MousePicker(Window* window, glm::mat4 const& projectionMatrix)
	:
		d_projectionMatrix(projectionMatrix),
		d_pWindow(window)
{}

glm::vec3 MousePicker::getRayPoint(glm::vec3 const& origin, glm::vec3 const& planePoint, glm::vec3 const& planeNormal)
{
	glm::vec3 normal = glm::normalize(planeNormal);
	float vdot = glm::dot(d_currentRayDir, normal);
	if(abs(vdot) > 0.0001f) // your favorite epsilon
	{
		float t = glm::dot(glm::vec3(0.f) - origin, normal) / vdot;
		if (t >= 0) return origin + d_currentRayDir * t;
	}

	return origin + d_currentRayDir * 10.f;
}

void MousePicker::update(glm::mat4 const& viewMatrix)
{
	// Get Normalized Device Coordinates
	glm::vec2 mousePos = d_pWindow->getCursorPos();
	float normX = (2.0f * mousePos.x) / d_pWindow->getWindowSize().x - 1.0f;
	float normY = 1.0f - (2.0f * mousePos.y) / d_pWindow->getWindowSize().y;

	glm::vec4 clipRay = glm::vec4(normX, normY, -1.0f, 1.0f);
	glm::vec4 eyeRay = glm::inverse(d_projectionMatrix) * clipRay;
	eyeRay = glm::vec4(eyeRay.x, eyeRay.y, -1.0f, 0.0f);
	
	d_currentRayDir = glm::normalize(glm::vec3(glm::inverse(viewMatrix)*eyeRay));
}