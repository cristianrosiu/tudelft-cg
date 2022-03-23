#include "entity.h"


unsigned const Entity::getHealth() const
{
	return m_health;
}

void Entity::setHealth(int modifier)
{
	m_health += modifier;
}