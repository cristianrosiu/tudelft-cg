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
		void createBoss(std::vector<std::filesystem::path> components);
		

	public:
		glm::mat3 getJacobian();
		std::queue<glm::vec3> getGradients();
		Boss(std::vector<std::filesystem::path> components, Player* player);
		void draw(Shader& shader);
		glm::vec3 const& getLastPosition();
		glm::vec3 const& getLastForward();
		void updateBoss(float deltaTime);
};

#endif