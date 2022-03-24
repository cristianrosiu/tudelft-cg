#include "boss.h"
#include "gameobject.h"
#include <iostream>

Boss::Boss(std::filesystem::path const& body, std::filesystem::path const& head, int const& layers, Player* player)
	: m_root(body), m_size(layers), m_player(player)
{
	createBoss(body, head);
}

void Boss::createBoss(std::filesystem::path const& body, std::filesystem::path const& head)
{
	GameObject* lastEntity = &m_root;

	for (int i = 0; i < m_size; ++i)
	{
		if (i == m_size-1)
			lastEntity->addChild(body);
		else
			lastEntity->addChild(body);
		
		lastEntity = lastEntity->children.back().get();
		lastEntity->transform.setLocalPosition(glm::vec3(0.f, 0.f, -1.f));
	}

	m_root.updateSelfAndChild();
}

glm::vec3 const& Boss::getLastPosition()
{
	GameObject* lastEntity = &m_root;

	while (lastEntity->children.size())
		lastEntity = lastEntity->children.back().get();

	return lastEntity->transform.getGlobalPosition();
}

glm::vec3 const& Boss::getLastForward()
{
	GameObject* lastEntity = &m_root;

	while (lastEntity->children.size())
		lastEntity = lastEntity->children.back().get();

	return lastEntity->transform.getForward();
}

glm::mat3x3 Boss::getJacobian()
{
	std::queue<glm::vec3> dms = Boss::getGradients();

	int i = 0;
	glm::mat3x3 jacobian{ 1.f};

	while (dms.size() != 0)
	{
		glm::vec3 dm = dms.front();
		dms.pop();

		jacobian[i] = dm;
		i++;
	}
	return jacobian;

}

void Boss::updateBoss(float deltaTime)
{
	glm::vec3 targetPos = m_player->transform.getLocalPosition();
	glm::vec3 lastPos = getLastPosition();


	glm::mat3 jacobian = getJacobian();
	glm::mat3 inverseJacobian{1.f};
	if (glm::determinant(jacobian))
		inverseJacobian = glm::inverse(jacobian * glm::transpose(jacobian));
	else
		inverseJacobian = glm::transpose(jacobian);
		

	float speed = (glm::distance(targetPos, lastPos) > 2.5f) ? 0.003f : 0.05f;
	glm::vec3 angles = inverseJacobian*((targetPos - lastPos)*speed);
	
	if (glm::distance(targetPos, lastPos) < 1.5f)
	{
		m_player->transform.setLocalPosition(m_player->transform.getLocalPosition() + getLastForward()*2.f);
		m_player->setHealth(-5);
	}
	
	int i = 0;
	GameObject* lastEntity = &m_root;
	while (lastEntity->children.size())
	{
		glm::vec3 currentRot = lastEntity->transform.getLocalRotation();
		lastEntity->transform.setLocalRotation(currentRot + glm::vec3(0.f, glm::degrees(angles[i]), 0.f));
		lastEntity = lastEntity->children.back().get();
		i++;
	}
	m_root.updateSelfAndChild();
	
}

void Boss::draw(Shader &shader)
{
	GameObject* lastEntity = &m_root;

	while (lastEntity->children.size())
	{
		shader.setMatrix("modelMatrix", lastEntity->transform.getModelMatrix());
		lastEntity->draw(shader);
		lastEntity = lastEntity->children.back().get();
	}
	shader.setMatrix("modelMatrix", lastEntity->transform.getModelMatrix());
	lastEntity->draw(shader);
}

std::queue<glm::vec3> Boss::getGradients()
{
	std::queue<glm::vec3> dms;
	GameObject* lastEntity = &m_root;

	glm::vec3 lastJointPos = getLastPosition();
	while (lastEntity->children.size())
	{
		glm::vec3 targetPos = lastJointPos - lastEntity->transform.getGlobalPosition();
		glm::vec3 dm = glm::cross(glm::vec3(0.f, 1.f, 0.f), targetPos);
		dms.push(dm);

		lastEntity = lastEntity->children.back().get();
	}

	return dms;
}