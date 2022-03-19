#include "../mesh.h"
#include "../texture.h"
#include <string>
#include <filesystem>
#include <glm/glm.hpp>

#ifndef Entity_H
#define Entity_H
class Entity {
protected:
    std::vector<GPUMesh> d_meshs;         // mesh of entity
    Texture d_texture {"resources/checkerboard.png"};      // texture of entity
    int d_health {100};           // health of entity
    std::string d_name {"Steve"};     // entity name
    glm::vec3 d_position { 0.f };   // current entity position
    glm::vec3 d_rotation { 0.f };   // current entity rotation
    glm::vec3 d_forward{ 0, 0, -1 };
    glm::vec3 d_up{ 0.f, 1.f, 0.f };

public:
    Entity(std::string const &name,
           int const &health,
           std::filesystem::path const &texture,
           std::filesystem::path const &mesh,
           glm::vec3 const &position,
           glm::vec3 const &rotation);
    
    void setName(std::string const &name);
    void setHealth(int const &modifier);
    
    glm::vec3 const &position() const;
    glm::vec3 const &rotation() const;
    glm::vec3 const& forward()  const;
    std::string const &name()   const;
    int const &health()         const;
    GPUMesh &mesh(int const& index);
    Texture &texture();
};

#endif