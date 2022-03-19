#include <glm/mat4x4.hpp>

struct Node {
    glm::mat4 transformationMatrix{ 1.f };

    /* Tree pointers. */
    struct Node* next_object;  /* Link to object at same level. */
    struct Node* next_level;   /* Link to object at next level. */
};