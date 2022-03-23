#include "gameobject.h"


#ifndef ENTITY_H
#define ENTITY_H

class Entity
{
	unsigned int m_health{ 100 };

	public:
		Entity() = default;
		
		void setHealth(int modifier);
		unsigned int const getHealth() const;

};

#endif