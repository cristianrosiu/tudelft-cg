#include "mesh.h"
#include <framework/disable_all_warnings.h>
#include <framework/mesh.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <imgui/imgui.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()
#include <iostream>
#include <vector>

GPUMesh::GPUMesh(std::filesystem::path filePath, std::filesystem::path texture)
    :
    d_diffuseTexture(texture)
{
    if (!std::filesystem::exists(filePath))
        throw MeshLoadingException(fmt::format("File {} does not exist", filePath.string().c_str()));

    const auto cpuMesh = mergeMeshes(loadMesh(filePath));
    
    this->d_vertices  = cpuMesh.vertices;
    this->d_triangles = cpuMesh.triangles;
    this->material = cpuMesh.material;

    std::cout << material.kd.x << " " << material.ks.x << " " << "\n\n";

    setupMesh();
}

GPUMesh::GPUMesh(std::filesystem::path filePath)
{
    if (!std::filesystem::exists(filePath))
        throw MeshLoadingException(fmt::format("File {} does not exist", filePath.string().c_str()));

    const auto cpuMesh = mergeMeshes(loadMesh(filePath));

    this->d_vertices = cpuMesh.vertices;
    this->d_triangles = cpuMesh.triangles;

    setupMesh();
}

void GPUMesh::setupMesh()
{
    // Create Element(/Index) Buffer Objects and Vertex Buffer Object.
    glCreateBuffers(1, &d_ibo);
    glNamedBufferStorage(d_ibo, static_cast<GLsizeiptr>(d_triangles.size() * sizeof(decltype(d_triangles)::value_type)), d_triangles.data(), 0);

    glCreateBuffers(1, &d_vbo);
    glNamedBufferStorage(d_vbo, static_cast<GLsizeiptr>(d_vertices.size() * sizeof(decltype(d_vertices)::value_type)), d_vertices.data(), 0);

    // Bind vertex data to shader inputs using their index (location).
    // These bindings are stored in the Vertex Array Object.
    glCreateVertexArrays(1, &d_vao);

    // The indices (pointing to vertices) should be read from the index buffer.
    glVertexArrayElementBuffer(d_vao, d_ibo);

    // We bind the vertex buffer to slot 0 of the VAO and tell the VBO how large each vertex is (stride).
    glVertexArrayVertexBuffer(d_vao, 0, d_vbo, 0, sizeof(Vertex));
    // Tell OpenGL that we will be using vertex attributes 0, 1 and 2.
    glEnableVertexArrayAttrib(d_vao, 0);
    glEnableVertexArrayAttrib(d_vao, 1);
    glEnableVertexArrayAttrib(d_vao, 2);
    // We tell OpenGL what each vertex looks like and how they are mapped to the shader (location = ...).
    glVertexArrayAttribFormat(d_vao, 0, 3, GL_FLOAT, 0, offsetof(Vertex, position));
    glVertexArrayAttribFormat(d_vao, 1, 3, GL_FLOAT, false, offsetof(Vertex, normal));
    glVertexArrayAttribFormat(d_vao, 2, 2, GL_FLOAT, false, offsetof(Vertex, texCoord));
    // For each of the vertex attributes we tell OpenGL to get them from VBO at slot 0.
    glVertexArrayAttribBinding(d_vao, 0, 0);
    glVertexArrayAttribBinding(d_vao, 1, 0);
    glVertexArrayAttribBinding(d_vao, 2, 0);

    // Each triangle has 3 vertices.
    d_numIndices = static_cast<GLsizei>(3 * d_triangles.size());

}

GPUMesh::GPUMesh(GPUMesh&& other)
{
    moveInto(std::move(other));
}

GPUMesh::~GPUMesh()
{
    freeGpuMemory();
}

GPUMesh& GPUMesh::operator=(GPUMesh&& other)
{
    moveInto(std::move(other));
    return *this;
}

bool GPUMesh::hasTextureCoords() const
{
    return d_hasTextureCoords;
}

void GPUMesh::draw(Shader &shader)
{
    glBindVertexArray(d_vao);
    glDrawElements(GL_TRIANGLES, d_numIndices, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}   

void GPUMesh::bindTexture(int slot, int location)
{
    glUniform1i(4, 0);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, d_diffuseTexture.getTextureID());
    glUniform1i(location, slot);
}

void GPUMesh::moveInto(GPUMesh&& other)
{
    freeGpuMemory();
    d_numIndices = other.d_numIndices;
    d_hasTextureCoords = other.d_hasTextureCoords;
    d_ibo = other.d_ibo;
    d_vbo = other.d_vbo;
    d_vao = other.d_vao;

    other.d_numIndices = 0;
    other.d_hasTextureCoords = other.d_hasTextureCoords;
    other.d_ibo = INVALID;
    other.d_vbo = INVALID;
    other.d_vao = INVALID;
}

void GPUMesh::freeGpuMemory()
{
    if (d_vao != INVALID)
        glDeleteVertexArrays(1, &d_vao);
    if (d_vbo != INVALID)
        glDeleteBuffers(1, &d_vbo);
    if (d_ibo != INVALID)
        glDeleteBuffers(1, &d_ibo);
}
