#include <glm/mat4x4.hpp>

struct Node {
    glm::mat4 translationMatrix{ 1.f };
    glm::mat4 rotationMatrix{ 1.f };

    float theta;

    glm::vec3 dm;

    /* Tree pointers. */
    struct Node* next_object;  /* Link to object at same level. */
    struct Node* next_level;   /* Link to object at next level. */
};