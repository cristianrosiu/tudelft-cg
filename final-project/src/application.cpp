//#include "Image.h"
#include "mousepicker.h"
// Always include window first (because it includes glfw, which includes GL which needs to be included AFTER glew).
// Can't wait for modules to fix this stuff...
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glad/glad.h>
// Include glad before glfw3
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include "glm/gtx/string_cast.hpp"
#include <imgui/imgui.h>
DISABLE_WARNINGS_POP()
#include <framework/shader.h>
#include <framework/window.h>
#include <functional>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include "shadowmap.h"
#include "utility.h"
#include "player.h"
#include "boss.h"

class Application {
public:
    Application()
        : m_window("Final Project", glm::ivec2(1024, 1024), OpenGLVersion::GL45)
    {
        m_window.registerKeyCallback([this](int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS)
                onKeyPressed(key, mods);
            else if (action == GLFW_RELEASE)
                onKeyReleased(key, mods);
        });
        m_window.registerMouseMoveCallback(std::bind(&Application::onMouseMove, this, std::placeholders::_1));
        m_window.registerMouseButtonCallback([this](int button, int action, int mods) {
            if (action == GLFW_PRESS)
                onMouseClicked(button, mods);
            else if (action == GLFW_RELEASE)
                onMouseReleased(button, mods);
        });

        try {
            m_defaultShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/shader_frag.glsl").build();
            m_shadowShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl").build();

            // Any new shaders can be added below in similar fashion.
            // ==> Don't forget to reconfigure CMake when you do!
             //     Visual Studio: PROJECT => Generate Cache for ComputerGraphics
            //     VS Code: ctrl + shift + p => CMake: Configure => enter
            // ....
        } catch (ShaderLoadingException e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void update()
    {
        GLfloat lastFrame = (GLfloat)glfwGetTime();

        // Main light
        m_lights[0].position = glm::vec3(5.f, 4.f, 5.f);
        m_lights[0].viewMatrix = glm::lookAt(m_lights[0].position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        m_lights[0].color = glm::vec3(1.f, 0.f, 0.0f);

        // Boss light
        m_lights[1].position = glm::vec3(-5.f, 4.f, -5.f);
        m_lights[1].viewMatrix = glm::lookAt(m_lights[1].position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        m_lights[1].color = glm::vec3(0.f, 0.f, 1.f);

        Player player{ "./resources/animation", &m_window, m_projectionMatrix };
        player.material.kd = glm::vec3(0.8f);
        player.material.ks = glm::vec3(0.5f);
        player.material.shininess = 32.f;

        Boss boss{ "./resources/boss/body", "./resources/boss/head", 3, &player};
        GameObject floor{ "./resources/floor" };
        floor.material.kd = glm::vec3(0.8f);
        floor.material.ks = glm::vec3(0.2f);
        floor.material.shininess = 10.f;


        while (!m_window.shouldClose()) {
            m_window.updateInput();


            m_lights[1].position = boss.getLastPosition() + glm::vec3(0.f, 4.f, 0.f);
            m_lights[1].viewMatrix = glm::lookAt(m_lights[1].position, player.transform.getGlobalPosition(), glm::vec3(0.f, 1.f, 0.f));

            // Calculate DeltaTime of current frame
            GLfloat currentFrame = (GLfloat)glfwGetTime();
            m_deltaTime = (currentFrame - lastFrame);
            lastFrame = currentFrame;

            // Compute the shadow map textures
            for (int i = 0; i < 2; i++)
                m_shadowMaps[i].renderShadowMap(m_shadowShader, m_projectionMatrix, m_lights[i], player, floor);

            m_defaultShader.bind();

            // Send the shadow map textures to GPU
            m_defaultShader.setSampler("texShadow[0]", m_shadowMaps[0].getTextureID(), 0);
            m_defaultShader.setSampler("texShadow[1]", m_shadowMaps[1].getTextureID(), 1);

            // Set viewport size
            glViewport(0, 0, m_window.getWindowSize().x, m_window.getWindowSize().y);

            //clear scene
            glClearDepth(1.0f);
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);

            m_camPos = player.transform.getLocalPosition() + glm::vec3(0.f, 4.f, -4.f);
            m_viewMatrix = glm::lookAt(m_camPos, player.transform.getLocalPosition(), glm::vec3(0.f, 1.f, 0.f));

            // Updates
            player.move(m_deltaTime);
            player.lookAt(m_camPos, m_viewMatrix);
            boss.updateBoss();

            m_defaultShader.setMatrix("projectionMatrix", m_projectionMatrix);
            m_defaultShader.setMatrix("viewMatrix", m_viewMatrix);
            m_defaultShader.setVector("camPos", m_camPos);

            for (int i = 0; i < 2; i++)
            {
                m_defaultShader.setVector("lights[" + std::to_string(i)+ "].position", m_lights[i].position);
                m_defaultShader.setVector("lights[" + std::to_string(i) + "].color", m_lights[i].color);
                m_defaultShader.setMatrix("lights[" + std::to_string(i) + "].viewMatrix", m_projectionMatrix * m_lights[i].viewMatrix * player.transform.getModelMatrix());
            }

            // Draw Player
            m_defaultShader.setMatrix("modelMatrix", player.transform.getModelMatrix());
            m_defaultShader.setVector("material.diffuse", player.material.kd);
            m_defaultShader.setVector("material.specular", player.material.ks);
            m_defaultShader.setFloat("material.shininess", player.material.shininess);
            //player.bindTexture(2, 3);
            player.draw(m_defaultShader);

            for (int i = 0; i < 2; i++)
            {
                m_defaultShader.setVector("lights[" + std::to_string(i) + "].position", m_lights[i].position);
                m_defaultShader.setVector("lights[" + std::to_string(i) + "].color", m_lights[i].color);
                m_defaultShader.setMatrix("lights[" + std::to_string(i) + "].viewMatrix", m_projectionMatrix * m_lights[i].viewMatrix * floor.transform.getModelMatrix());
            }
            // Draw floor
            m_defaultShader.setVector("material.diffuse", floor.material.kd);
            m_defaultShader.setVector("material.specular", floor.material.ks);
            m_defaultShader.setFloat("material.shininess", floor.material.shininess);
            m_defaultShader.setMatrix("modelMatrix", floor.transform.getModelMatrix());
            //floor.bindTexture(3, 3);
            floor.draw(m_defaultShader);

            boss.draw(m_defaultShader);


            // Processes input and swaps the window buffer
            m_window.swapBuffers();
        }
    }

    // In here you can handle key presses
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyPressed(int key, int mods)
    {

    }

    // In here you can handle key releases
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyReleased(int key, int mods)
    {

    }

    // If the mouse is moved this function will be called with the x, y screen-coordinates of the mouse
    void onMouseMove(const glm::dvec2& cursorPos)
    {

    }

    // If one of the mouse buttons is pressed this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseClicked(int button, int mods)
    {

        
    }

    // If one of the mouse buttons is released this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/grodup__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseReleased(int button, int mods)
    {
    }

    void createShadowMap(int width, int height)
    {

    }



private:
    Window m_window;

    // Shader for default rendering and for depth rendering
    Shader m_defaultShader;
    Shader m_shadowShader;

    ShadowMap m_shadowMaps[2]{ ShadowMap{glm::uvec2(2048)}, ShadowMap{glm::uvec2(2048)} };
    Light m_lights[2];

    GLfloat m_deltaTime;

    // Projection and view matrices for you to fill in and use
    glm::mat4 m_projectionMatrix = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 30.0f);
    glm::mat4 m_viewMatrix = glm::lookAt(glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec3(0, 1, 0));
    glm::mat4 m_modelMatrix { 1.0f };

    glm::vec3 m_camPos{ 0.f };
};

int main()
{
    Application app;
    app.update();

    return 0;
}
