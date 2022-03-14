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
#include <framework/trackball.h>
#include <iostream>
#include <span>
#include <vector>
#include <algorithm>
using namespace std;

// --- Configuration ---
// Constants
constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;
constexpr int MAX_RADIUS = 50;
constexpr float ROTATION_ANGLE = 10.0f;
constexpr float EPSILON_FACTOR = 0.01f;
constexpr float ALPHA_FACTOR = 0.005f;
constexpr float PLANE_TRANS_FACTOR = 0.01f;
// Variables
bool project = false;
bool projectMiss = false;
float EPSILON = 0.0f;
float ALPHA = 0.0f;
bool plane = true;
bool plane2 = true;

int axisIndex = 0;
std::vector<glm::vec3> axis{ glm::vec3(1.0, 0.0, 0.0),  glm::vec3(0.0, 1.0, 0.0),  glm::vec3(0.0, 0.0, 1.0)};

/* Rotates a plane mesh given an angle and rotation axis */
void rotatePlane(Mesh& plane, float angle, glm::vec3 axis)
{
    // Get cen
    auto center = ((plane.vertices[0].position + plane.vertices[1].position) / 2.0f + (plane.vertices[2].position + plane.vertices[3].position) / 2.0f) / 2.0f;
    for (int i = 0; i < plane.vertices.size(); i++)
    {
        glm::mat4 translateToCenter = glm::translate(glm::mat4(1.0f), -center);
        glm::mat4 translateBackFromCenter = glm::translate(glm::mat4(1.0f), center);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), float(glm::radians(angle)), axis);
        glm::mat4 transformation = translateBackFromCenter * rotation * translateToCenter;

        plane.vertices[i].position = glm::vec3(transformation * glm::vec4(plane.vertices[i].position, 1.0f));
        plane.vertices[i].normal = glm::vec3(transformation * glm::vec4(plane.vertices[i].normal, 1.0f));
    }
}

/* Prints the help menu */
static void printHelp()
{
    std::cout << std::endl;
    std::cout << "Program Usage:" << std::endl;
    std::cout << "1 - Main Camera" << std::endl;
    std::cout << "2 - Side Camera" << std::endl;
    std::cout << "______________________" << std::endl;
    std::cout << "Left/Right Arrow - Move plane forward/backward" << std::endl;
    std::cout << "Scroll Wheel - Rotate plane" << std::endl;
    std::cout << "Press Scroll Wheel - Change rotation axis" << std::endl;
    std::cout << "______________________" << std::endl;
    std::cout << "Up/Down Arrow - Increase/Decrease EPSILON" << std::endl;
    std::cout << "SHIFT + Left/Right Arrow - Decrease/Increase ALPHA" << std::endl;
    std::cout << "z - Project VALID Set" << std::endl;
    std::cout << "x - Project MISS Set" << std::endl;
    std::cout << "c - Drop Projection" << std::endl;

}

int main()
{
    printHelp();
    
    // Create main window
    Window window { "Billboard", glm::ivec2(WIDTH, HEIGHT), OpenGLVersion::GL45 };

    // Camera Setup
    Camera camera{ &window,  glm::vec3(1.2f, 1.1f, 0.9f), -glm::vec3(1.2f, 1.1f, 0.9f) };
    Camera sideCamera{ &window,  glm::vec3(3.0f, 0.0f, 0.3f), -glm::vec3(3.0f, 0.0f, 0.0f) };
    std::vector<Camera> cameras{ camera, sideCamera };
    int activeCamera = 0;

    constexpr float fov = glm::pi<float>() / 4.0f;
    constexpr float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    const glm::mat4 mainProjectionMatrix = glm::perspective(fov, aspect, 0.1f, 90.0f);
    const glm::mat4 orthoMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 3.0f);

    // Create all shaders
    const Shader objShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/obj_shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/obj_shader_frag.glsl").build();
    const Shader planeShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/plane_shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/plane_shader_frag.glsl").build();
    const Shader planeShader2 = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/plane_shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/plane_shader_frag.glsl").build();
    
    // Load mesh from disk.
    auto mesh = loadMesh("resources/dragon3.obj");

    // Split mesh into obj + 2 planes
    auto planeMesh = mesh[1];
    auto objMesh = mesh[0];
    auto planeMesh2 = mesh[2];

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Setup meshes
    planeMesh.setupMesh();
    planeMesh2.setupMesh();
    objMesh.setupMesh();

    // Key handle function
    window.registerKeyCallback([&](int key, int /* scancode */, int action, int /* mods */) {
        switch (key) {
        case GLFW_KEY_1:
            if (action == GLFW_RELEASE)
                activeCamera = 0;
            break;
        case GLFW_KEY_2:
            if (action == GLFW_RELEASE)
                activeCamera = 1;
            break;
        default:
            break;
        }
    });

    // Mouse button handle function
    window.registerMouseButtonCallback([&](int button, int action, int mods){
        switch (button)
        {
            case GLFW_MOUSE_BUTTON_3:
                axisIndex = (axisIndex + 1) % axis.size();
                break;
        }
    });
    
    // Mouse scroll handle function
    window.registerScrollCallback([&](const glm::vec2& offset) {
        if (offset.g < 0)
        {
            rotatePlane(planeMesh, -ROTATION_ANGLE, axis[axisIndex]);
            rotatePlane(planeMesh2, -ROTATION_ANGLE, axis[axisIndex]);
        }
        else
        {
            rotatePlane(planeMesh, ROTATION_ANGLE, axis[axisIndex]);
            rotatePlane(planeMesh2, ROTATION_ANGLE, axis[axisIndex]);
        }
    });

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

        // ----- Object Shader -----
        objShader.bind();

        // Setup Uniforms
        if (activeCamera == 0)
            cameras[activeCamera].updateInput();

        glm::mat4 mvp = mainProjectionMatrix * cameras[activeCamera].viewMatrix(); // Assume model matrix is identity.
        if (activeCamera == 1)
            mvp = orthoMatrix * cameras[activeCamera].viewMatrix();
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

        const glm::vec3 cameraPos = cameras[activeCamera].cameraPos();
        glUniform3fv(8, 1, glm::value_ptr(cameraPos));

        auto t = planeMesh.triangles[0];
        auto edge1 = planeMesh.vertices[t.g].position - planeMesh.vertices[t.r].position;
        auto edge2 = planeMesh.vertices[t.b].position - planeMesh.vertices[t.r].position;
        glm::vec3 planeNormal = glm::normalize(glm::cross(edge1, edge2));

        glUniform3fv(2, 1, glm::value_ptr(planeMesh.vertices[0].position));
        glUniform3fv(6, 1, glm::value_ptr(planeMesh2.vertices[0].position));
        glUniform3fv(3, 1, glm::value_ptr(planeNormal));
        glUniform1i(7, plane2);
        glUniform1i(4, project);
        glUniform1i(9, projectMiss);
        glUniform1f(5, EPSILON);
        
        // Key handlers
        bool shiftPressed = window.isKeyPressed(GLFW_KEY_LEFT_SHIFT) || window.isKeyPressed(GLFW_KEY_RIGHT_SHIFT);
        if (window.isKeyPressed(GLFW_KEY_RIGHT)) {
            if (shiftPressed)
            {
                ALPHA += ALPHA_FACTOR;
                //std::cout << "ALPHA: " << ALPHA << std::endl;
            }
            else {
                planeMesh.translate(PLANE_TRANS_FACTOR, glm::vec3(0.0f, 0.0f, 1.0f));
                planeMesh2.translate(PLANE_TRANS_FACTOR, glm::vec3(0.0f, 0.0f, 1.0f));
            }
        }
        else if (window.isKeyPressed(GLFW_KEY_LEFT)) {
            if (shiftPressed) 
            {
                ALPHA -= ALPHA_FACTOR;
                //std::cout << "ALPHA: " << ALPHA << std::endl;
            }
            else {
                planeMesh.translate(PLANE_TRANS_FACTOR, glm::vec3(0.0f, 0.0f, -1.0f));
                planeMesh2.translate(PLANE_TRANS_FACTOR, glm::vec3(0.0f, 0.0f, -1.0f));
            }
        }
        else if (window.isKeyPressed(GLFW_KEY_Z)) {
            project = true;
            plane = true;
            plane2 = false;
            projectMiss = false;
        }
        else if (window.isKeyPressed(GLFW_KEY_X)) {
            projectMiss = true;
            plane2 = true;
            plane = false;
            project = false;
        }
        else if (window.isKeyPressed(GLFW_KEY_C)) {
            project = false;
            projectMiss = false;
            plane2 = true;
            plane = true;
        }
        else if (window.isKeyPressed(GLFW_KEY_UP)) {
            EPSILON = std::max(0.0f, std::min(EPSILON + EPSILON_FACTOR, (float)MAX_RADIUS));
            //std::cout << "EPSILON: " << EPSILON << std::endl;
        }
        else if (window.isKeyPressed(GLFW_KEY_DOWN)) {
            EPSILON = std::max(0.0f, std::min(EPSILON - EPSILON_FACTOR, (float)MAX_RADIUS));
            //std::cout << "EPSILON: " << EPSILON << std::endl;
        }

        // Clamp alpha and move the second plane
        ALPHA = std::max(0.0f, std::min(ALPHA, EPSILON));
        for (auto it = planeMesh2.vertices.begin(); it != planeMesh2.vertices.end(); ++it) {
            int i = std::distance(planeMesh2.vertices.begin(), it);
            planeMesh2.vertices[i].position = planeMesh.vertices[i].position + ALPHA * -planeNormal;
        }
        
        // Draw the object
        objMesh.draw();
       
        // Draw Main plane
        if (plane) {
            planeMesh.setupMesh();
            planeShader.bind();
            glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));
            glUniform4fv(2, 1, glm::value_ptr(glm::vec4(0.0, 0.5, 0.0, 0.5)));
            planeMesh.draw();
        }

        // Draw secondary plane
        if (plane2) {
            planeMesh2.setupMesh();
            planeShader2.bind();
            glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));
            auto color = glm::vec4(1.0f, 0.2f, 0.0f, 0.5f);
            glUniform4fv(2, 1, glm::value_ptr(color));
            planeMesh2.draw();
        }

        // Present result to the screen.
        window.swapBuffers();
    }

    // Clean mesh data
    objMesh.clearMesh();
    planeMesh.clearMesh();
    planeMesh2.clearMesh();

    return 0;
}

