#include "include/entity.h"
#include <utility>

Entity::Entity(std::string const &name, int const &health, 
			   std::filesystem::path const& texture, 
	           std::filesystem::path const& mesh, 
	           glm::vec3 const& position, glm::vec3 const& rotation)
:
	d_name(name),
	d_health(health),
	d_texture(texture),
	d_mesh(mesh),
	d_position(position),
	d_rotation(rotation)
{}

// Setters
void Entity::setName(std::string const &name)
{
	d_name = name;
}
void Entity::setHealth(int const &modifier)
{
	d_health = std::max(0, d_health + modifier);
}
void Entity::increasePosition(glm::vec3 const &v)
{
	d_position += v;
}
void Entity::increaseRotation(glm::vec3 const &v)
{
	d_rotation += v;
}

// Getters
std::string const &Entity::name() const
{
	return d_name;
}
int const &Entity::health() const
{
	return d_health;
}
GPUMesh &Entity::mesh()
{
	return d_mesh;
}
Texture& Entity::texture()
{
	return d_texture;
}
glm::vec3 const &Entity::position() const
{
	return d_position;
}
glm::vec3 const &Entity::rotation() const
{
	return d_rotation;
}
