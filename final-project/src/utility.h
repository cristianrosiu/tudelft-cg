#include <glm/glm.hpp>

struct Light {
    glm::vec3 position;
    glm::mat4 viewMatrix;
    glm::vec3 direction;
    glm::vec3 color;
};

//struct Material {
//    glm::vec3 ambient;
//    glm::vec3 diffuse;
//    glm::vec3 specular;
//    float shininess;
//};