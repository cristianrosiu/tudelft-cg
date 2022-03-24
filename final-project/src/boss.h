#include "gameobject.h"
#include "utility.h"
#include <queue>
#include "player.h"

#ifndef BOSS_H
#define BOSS_H


class Boss: public Entity
{
	GameObject m_root;
	//GameObject* lastEntity;

	Player* m_player;

	unsigned int m_size{ 2 };

	private:
		void createBoss(std::filesystem::path const& body, std::filesystem::path const& head);
		

	public:
		glm::mat3 getJacobian();
		std::queue<glm::vec3> getGradients();
		Boss(std::filesystem::path const& body, std::filesystem::path const& head, int const& layers, Player* player);
		void draw(Shader& shader);
		glm::vec3 const& getLastPosition();
		glm::vec3 const& getLastForward();
		void updateBoss(float deltaTime);
};

#endif