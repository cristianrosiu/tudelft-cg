#include "boss.h"

Boss::Boss(std::filesystem::path const& body, std::filesystem::path const& head, int layers)
	: m_root(body)
{
	//createBoss(body, head, layers);
}

void Boss::createBoss(std::filesystem::path const& body, std::filesystem::path const& head, int layers)
{
	GameObject* lastEntity = &m_root;

	for (int i = 0; i < layers; i++)
	{
		if (i == layers - 1)
			lastEntity->addChild(head);
		else
			lastEntity->addChild(body);

		lastEntity = lastEntity->children.back().get();

		lastEntity->transform.setLocalPosition(glm::vec3(0.f, 0.f, -1.f));
	}

	lastEntity = nullptr;
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

	lastEntity = nullptr;
}