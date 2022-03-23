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
	lastEntity = &m_root;

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
	lastEntity = &m_root;

	while (lastEntity->children.size())
		lastEntity = lastEntity->children.back().get();

	return lastEntity->transform.getGlobalPosition();
}

glm::mat3 Boss::getJacobian()
{
	std::queue<glm::vec3> dms = getGradients();
	int i = 0;
	glm::mat3 jacobian{ 1.f };

	while (dms.size())
	{
		glm::vec3 dm = dms.front();
		dms.pop();

		jacobian[i] = dm;
		i++;
	}
	return jacobian;

}

void Boss::updateBoss()
{
	glm::vec3 targetPos = m_player->transform.getGlobalPosition();
	glm::vec3 lastPos = getLastPosition();

	glm::mat3 jacobian = getJacobian();
	auto jacobianPseudoInverse = glm::inverse(jacobian * glm::transpose(jacobian)) * 0.01f * (targetPos - lastPos);
	glm::vec3 angles = glm::transpose(jacobian) * jacobianPseudoInverse;
	std::cout << glm::distance(glm::normalize(targetPos), glm::normalize(lastPos)) << "\n\n";
	if (glm::distance(targetPos, lastPos) < 0.5f)
		return;

	int i = 0;
	lastEntity = &m_root;
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
	lastEntity = &m_root;

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
	lastEntity = &m_root;

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