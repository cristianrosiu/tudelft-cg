#include <glm/mat4x4.hpp>
#include "mesh.h"
static enum NodeType { BODY, HEAD };

struct Node {
    glm::mat4 translationMatrix{ 1.f };
    glm::mat4 rotationMatrix{ 1.f };

    glm::vec3 dm;

    NodeType type;

    GPUMesh mesh{"resources/dragon.obj"};

    /* Tree pointers. */
    struct Node* next_object;  /* Link to object at same level. */
    struct Node* next_level;   /* Link to object at next level. */
};