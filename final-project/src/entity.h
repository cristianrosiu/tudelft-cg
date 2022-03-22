#include "gameobject.h"


#ifndef ENTITY_H
#define ENTITY_H

class Entity : public GameObject
{
	int m_health{ 0 };

	public:
		Entity(std::filesystem::path const& path);
		
		void setHealth(int modifier);
		int const getHealth() const;

};

#endif