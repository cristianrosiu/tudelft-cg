#include "include/entity.h"
#include <utility>
#include <iostream>

Entity::Entity(std::string const &name, int const &health, 
			   std::filesystem::path const& texture,
			   std::filesystem::path const& sTexture,
	           std::filesystem::path const& mesh, 
	           glm::vec3 const& position, glm::vec3 const& rotation)
:
	d_name(name),
	d_health(health),
	d_texture(texture),
	d_sTexture(sTexture),
	d_position(position),
	d_rotation(rotation)
{
	for (const auto& entry : std::filesystem::directory_iterator("./resources/animation"))
	{
		if (entry.path().extension() == ".obj")
		{
			std::cout << entry.path() << "\n";
			d_meshs.push_back(GPUMesh{ entry.path() });
		}
	}
	std::cout << d_meshs.size() << "\n";
		
}

// Setters
void Entity::setName(std::string const &name)
{
	d_name = name;
}
void Entity::setHealth(int const &modifier)
{
	d_health = std::max(0, d_health + modifier);
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
GPUMesh &Entity::mesh(int const &index)
{
	return d_meshs[index];
}
Texture& Entity::texture()
{
	return d_texture;
}
Texture& Entity::sTexture()
{
	return d_sTexture;
}
glm::vec3 const &Entity::position() const
{
	return d_position;
}
glm::vec3 const& Entity::forward() const
{
	return d_forward;
}
glm::vec3 const &Entity::rotation() const
{
	return d_rotation;
}
