#include "../mesh.h"
#include "../texture.h"
#include <string>
#include <filesystem>
#include <glm/glm.hpp>

class Entity {
protected:
    GPUMesh d_mesh;         // mesh of entity
    Texture d_texture;      // texture of entity
    int d_health;           // health of entity
    std::string d_name;     // entity name
    glm::vec3 d_position;   // current entity position
    glm::vec3 d_rotation;   // current entity rotation

public:
    Entity(std::string const &name = "Steve",
           int const &health = 100,
           std::filesystem::path const &texture = { "resources/checkerboard.png" },
           std::filesystem::path const &mesh = { "resources/dragon.obj" },
           glm::vec3 const &position = glm::vec3(0.0f),
           glm::vec3 const &rotation = glm::vec3(0.0f));
    
    void setName(std::string const &name);
    void setHealth(int const &modifier);
    void increasePosition(glm::vec3 const &v);
    void increaseRotation(glm::vec3 const& v);
    
    glm::vec3 const &position() const;
    glm::vec3 const &rotation() const;
    std::string const &name()   const;
    int const &health()         const;
    GPUMesh &mesh();
    Texture &texture();
};