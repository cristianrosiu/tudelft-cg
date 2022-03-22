#pragma once
#include <exception>
#include <filesystem>
#include <framework/opengl_includes.h>
#include <vector>
#include <framework/mesh.h>
#include <framework/shader.h>
#include "texture.h"

struct MeshLoadingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

#ifndef GPUMESH_H
#define GPUMESH_H

class GPUMesh {

    GLsizei d_numIndices{ 0 };
    bool d_hasTextureCoords{ false };
    static constexpr GLuint INVALID = 0xFFFFFFFF;
    GLuint d_ibo{ INVALID };
    GLuint d_vbo{ INVALID };
    GLuint d_vao{ INVALID };

    std::vector<Vertex> d_vertices;
    std::vector<glm::uvec3> d_triangles;
    Texture d_diffuseTexture{"resources/checkerboard.png"};

public:
    GPUMesh(std::filesystem::path filePath, std::filesystem::path texture);
    GPUMesh(std::filesystem::path filePath);
    // Cannot copy a GPU mesh because it would require reference counting of GPU resources.
    GPUMesh(const GPUMesh&) = delete;
    GPUMesh(GPUMesh&&);
    ~GPUMesh();

    // Cannot copy a GPU mesh because it would require reference counting of GPU resources.
    GPUMesh& operator=(const GPUMesh&) = delete;
    GPUMesh& operator=(GPUMesh&&);

    bool hasTextureCoords() const;

    // Bind VAO and call glDrawElements.
    void draw(Shader& shader);
    void bindTexture(int slot, int location);

private:
    void setupMesh();
    void moveInto(GPUMesh&&);
    void freeGpuMemory();

private:

};

#endif
