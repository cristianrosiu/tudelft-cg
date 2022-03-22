#include "gameobject.h"

class Boss
{
	GameObject m_root;

	private:
		void createBoss(std::filesystem::path const& body, std::filesystem::path const& head, int layers);

	public:
		Boss(std::filesystem::path const& body, std::filesystem::path const& head, int layers);
		void draw(Shader &shader);
};