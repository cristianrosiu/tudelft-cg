#include "camera.h"
// Suppress warnings in third-party code.
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
// Include GLEW before GLFW
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
// Library for loading an image
#include <stb/stb_image.h>
DISABLE_WARNINGS_POP()
#include <array>
#include <framework/mesh.h>
#include <framework/shader.h>
#include <framework/window.h>
#include <iostream>
#include <span>
#include <vector>

// Configuration
constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;


int main()
{
    Window window { "Shadow Mapping", glm::ivec2(WIDTH, HEIGHT), OpenGLVersion::GL45 };

    Camera camera0{ &window,  glm::vec3(1.2f, 1.1f, 0.9f), -glm::vec3(1.2f, 1.1f, 0.9f) };
    Camera camera1{ &window, glm::vec3(-1.2f, 1.1f, 0.9f), -glm::vec3(-1.2f, 1.1f, 0.9f) };
    std::vector<Camera> cameras;
    cameras.push_back(camera0);
    cameras.push_back(camera1);

    int activeCam = 0;

    constexpr float fov = glm::pi<float>() / 4.0f;
    constexpr float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    const glm::mat4 mainProjectionMatrix = glm::perspective(fov, aspect, 0.1f, 30.0f);

    // === Modify for exercise 1 ===
    // Key handle function
    window.registerKeyCallback([&](int key, int /* scancode */, int action, int /* mods */) {
        switch (key) {
        case GLFW_KEY_1:
            activeCam = 0;
            break;
        case GLFW_KEY_2:
            activeCam = 1;
            break;
        default:
            break;
        }
    });

    const Shader mainShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/shader_frag.glsl").build();
    const Shader shadowShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl").build();

    // === Load a texture for exercise 5 ===
    // Create Texture
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("resources/smiley.png", &texWidth, &texHeight, &texChannels, 3);

    GLuint texLight;
    glCreateTextures(GL_TEXTURE_2D, 1, &texLight);
    glTextureStorage2D(texLight, 1, GL_RGB8, texWidth, texHeight);
    glTextureSubImage2D(texLight, 0, 0, 0, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Set behaviour for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(texLight, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texLight, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(texLight, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texLight, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load mesh from disk.
    const Mesh mesh = mergeMeshes(loadMesh("resources/scene.obj"));

    // Create Element(Index) Buffer Object and Vertex Buffer Objects.
    GLuint ibo;
    glCreateBuffers(1, &ibo);
    glNamedBufferStorage(ibo, static_cast<GLsizeiptr>(mesh.triangles.size() * sizeof(decltype(Mesh::triangles)::value_type)), mesh.triangles.data(), 0);

    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, static_cast<GLsizeiptr>(mesh.vertices.size() * sizeof(Vertex)), mesh.vertices.data(), 0);

    // Bind vertex data to shader inputs using their index (location).
    // These bindings are stored in the Vertex Array Object.
    GLuint vao;
    glCreateVertexArrays(1, &vao);

    // The indices (pointing to vertices) should be read from the index buffer.
    glVertexArrayElementBuffer(vao, ibo);

    // The position and normal vectors should be retrieved from the specified Vertex Buffer Object.
    // The stride is the distance in bytes between vertices. We use the offset to point to the normals
    // instead of the positions.
    glVertexArrayVertexBuffer(vao, 0, vbo, offsetof(Vertex, position), sizeof(Vertex));
    glVertexArrayVertexBuffer(vao, 1, vbo, offsetof(Vertex, normal), sizeof(Vertex));
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);

    // === Create Shadow Texture ===
    GLuint texShadow;
    const int SHADOWTEX_WIDTH = 1024;
    const int SHADOWTEX_HEIGHT = 1024;
    glCreateTextures(GL_TEXTURE_2D, 1, &texShadow);
    glTextureStorage2D(texShadow, 1, GL_DEPTH_COMPONENT32F, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);

    // Set behaviour for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(texShadow, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texShadow, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(texShadow, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texShadow, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // === Create framebuffer for extra texture ===
    GLuint framebuffer;
    glCreateFramebuffers(1, &framebuffer);
    glNamedFramebufferTexture(framebuffer, GL_DEPTH_ATTACHMENT, texShadow, 0);

    // Main loop
    while (!window.shouldClose()) {
        window.updateInput();

        // === Stub code for you to fill in order to render the shadow map ===
        {
            // Bind the off-screen framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            // Clear the shadow map and set needed options
            glClearDepth(1.0f);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            // Bind the shader
            shadowShader.bind();

            // Set viewport size
            glViewport(0, 0, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);

            // .... HERE YOU MUST ADD THE CORRECT UNIFORMS FOR RENDERING THE SHADOW MAP
            const glm::mat4 lightMVP = mainProjectionMatrix * cameras[1 - activeCam].viewMatrix();
            glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(lightMVP));

            // Bind vertex data
            glBindVertexArray(vao);

            // Execute draw command
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.triangles.size() * 3), GL_UNSIGNED_INT, nullptr);

            // Unbind the off-screen framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Bind the shader
        mainShader.bind();

        cameras[activeCam].updateInput();

        const glm::mat4 mvp = mainProjectionMatrix * cameras[activeCam].viewMatrix(); // Assume model matrix is identity.
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

        // Set view position
        const glm::vec3 cameraPos = cameras[activeCam].cameraPos();
        glUniform3fv(1, 1, glm::value_ptr(cameraPos));

        // .... HERE YOU MUST ADD THE CORRECT UNIFORMS FOR RENDERING THE MAIN IMAGE
        const glm::mat4 lightMVP = mainProjectionMatrix * cameras[1 - activeCam].viewMatrix(); // Assume model matrix is identity.
        glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(lightMVP));

        const glm::vec3 lightPos = cameras[1 - activeCam].cameraPos();
        glUniform3fv(4, 1, glm::value_ptr(lightPos));

        // Bind vertex data
        glBindVertexArray(vao);

        // Bind the shadow map to texture slot 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texShadow);
        glUniform1i(2, 0);

        // Bind the light texture to texture slot 0
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texLight);
        glUniform1i(5, 1);

        // Set viewport size
        glViewport(0, 0, WIDTH, HEIGHT);

        // Clear the framebuffer to black and depth to maximum value
        glClearDepth(1.0f);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // Execute draw command
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.triangles.size() * 3), GL_UNSIGNED_INT, nullptr);

        // Present result to the screen.
        window.swapBuffers();
    }

    // Be a nice citizen and clean up after yourself.
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &texShadow);
    glDeleteTextures(1, &texLight);
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}

