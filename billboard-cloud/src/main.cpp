#include "camera.h"
// Suppress warnings in third-party code.
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
// Include GLEW before GLFW
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include "glm/gtx/string_cast.hpp"
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
    Window window { "Billboard", glm::ivec2(WIDTH, HEIGHT), OpenGLVersion::GL45 };

    Camera camera{ &window,  glm::vec3(1.2f, 1.1f, 0.9f), -glm::vec3(1.2f, 1.1f, 0.9f) };
    std::vector<Camera> cameras;

    constexpr float fov = glm::pi<float>() / 4.0f;
    constexpr float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    const glm::mat4 mainProjectionMatrix = glm::perspective(fov, aspect, 0.1f, 30.0f);

    // === Modify for exercise 1 ===
    // Key handle function
    window.registerKeyCallback([&](int key, int /* scancode */, int action, int /* mods */) {
        switch (key) {
        case GLFW_KEY_1:
            break;
        case GLFW_KEY_2:
            break;
        default:
            break;
        }
    });

    const Shader objShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/obj_shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/obj_shader_frag.glsl").build();
    //const Shader planeShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/plane_shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/plane_shader_frag.glsl").build();
    
    // Load mesh from disk.
    auto mesh = loadMesh("resources/dragon2.obj");

    auto planeMesh = mesh[1];
    auto objMesh = mesh[0];

    planeMesh.setupMesh();
    objMesh.setupMesh(); 

    // Main loop
    while (!window.shouldClose()) {
        window.updateInput();

        // Set viewport size
        glViewport(0, 0, WIDTH, HEIGHT);

        // Clear the framebuffer to black and depth to maximum value
        glClearDepth(1.0f);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        objShader.bind();

        camera.updateInput();
        if (window.isKeyPressed(GLFW_KEY_B))
            planeMesh.vertices[0].position -= 0.01f * glm::vec3(0.0f, 0.0f, 1.0f);
        else if(window.isKeyPressed(GLFW_KEY_C))
            planeMesh.vertices[0].position -= 0.01f * glm::vec3(0.0f, 0.0f, -1.0f);

        const glm::mat4 mvp = mainProjectionMatrix * camera.viewMatrix(); // Assume model matrix is identity.
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

        // Set view position
        const glm::vec3 cameraPos = camera.cameraPos();
        glUniform3fv(1, 1, glm::value_ptr(cameraPos));

        // Draw Object
        glUniform3fv(2, 1, glm::value_ptr(planeMesh.vertices[0].position));
        glUniform3fv(3, 1, glm::value_ptr(planeMesh.vertices[0].normal));

        objMesh.draw();

        // Draw Plane
        //initShader(planeShader, camera, mainProjectionMatrix, glm::mat4(1.0));
        //planeMesh.draw();

        // Present result to the screen.
        window.swapBuffers();
    }

    objMesh.clearMesh();
    planeMesh.clearMesh();

    return 0;
}

