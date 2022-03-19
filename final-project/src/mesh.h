#pragma once
#include <exception>
#include <filesystem>
#include <framework/opengl_includes.h>
#include <vector>
#include <framework/mesh.h>
#include <framework/shader.h>

struct MeshLoadingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class GPUMesh {

    GLsizei d_numIndices{ 0 };
    bool d_hasTextureCoords{ false };
    static constexpr GLuint INVALID = 0xFFFFFFFF;
    GLuint d_ibo{ INVALID };
    GLuint d_vbo{ INVALID };
    GLuint d_vao{ INVALID };

public:
    GPUMesh(std::filesystem::path filePath);
    // Cannot copy a GPU mesh because it would require reference counting of GPU resources.
    GPUMesh(const GPUMesh&) = delete;
    GPUMesh(GPUMesh&&);
    ~GPUMesh();

    // Cannot copy a GPU mesh because it would require reference counting of GPU resources.
    GPUMesh& operator=(const GPUMesh&) = delete;
    GPUMesh& operator=(GPUMesh&&);

    bool hasTextureCoords() const;

    std::vector<Vertex> d_vertices;
    std::vector <glm::uvec3> d_triangles;

    // Bind VAO and call glDrawElements.
    void draw();
    void setupMesh();
    void bindMesh();

private:
    void moveInto(GPUMesh&&);
    void freeGpuMemory();

private:

};
