#include "gameobject.h"
#include "utility.h"
#include <queue>
#include "player.h"

class Boss
{
	GameObject m_root;
	GameObject* lastEntity;

	Player* m_player;

	unsigned int m_size{ 2 };

	private:
		std::queue<glm::vec3> getGradients();
		void createBoss(std::filesystem::path const& body, std::filesystem::path const& head);
		

	public:
		glm::mat3 getJacobian();
		Boss(std::filesystem::path const& body, std::filesystem::path const& head, int const& layers, Player* player);
		void draw(Shader& shader);
		glm::vec3 const& getLastPosition();
		void updateBoss();
};