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
	Texture baseColorTexture;
	Texture specularTexture;

	unsigned int m_size{ 2 };

	private:
		void createBoss(std::vector<std::filesystem::path> components);
		

	public:
		glm::mat3 getJacobian();
		std::queue<glm::vec3> getGradients();
		Boss(std::vector<std::filesystem::path> components, Player* player,
			std::filesystem::path const& baseColorTexture, std::filesystem::path const& specularTexture);
		void draw(Shader& shader);
		glm::vec3 const& getLastPosition();
		glm::vec3 const& getLastForward();
		void updateBoss(float deltaTime);
		void idleBoss();
		glm::vec3 getRootPosition();

		int getBaseColorTexture();
		int getSpecularTexture();
};

#endif