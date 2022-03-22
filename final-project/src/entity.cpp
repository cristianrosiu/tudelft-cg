#include "entity.h"

Entity::Entity(std::filesystem::path const& path)
	: GameObject(path)
{}

int const Entity::getHealth() const
{
	return m_health;
}

void Entity::setHealth(int modifier)
{
	m_health += modifier;
}