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
//#include "utility.h"
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

        m_light.position = glm::vec3(3.f, 10.f, 0.f);
        m_light.viewMatrix = glm::lookAt(m_light.position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

        ShadowMap shadowMap{ glm::uvec2(2048, 2048) };

        Player player{ "./resources/animation", &m_window, m_projectionMatrix };
        Boss boss{ "./resources/boss/body", "./resources/boss/head", 3};
        GameObject floor{ "./resources/floor" };

        while (!m_window.shouldClose()) {
            m_window.updateInput();

            // Calculate DeltaTime of current frame
            GLfloat currentFrame = (GLfloat)glfwGetTime();
            m_deltaTime = (currentFrame - lastFrame);
            lastFrame = currentFrame;

            shadowMap.renderShadowMap(m_shadowShader, m_projectionMatrix, m_light, player, floor);

            m_defaultShader.bind();

            shadowMap.bind(0, 6);

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

            player.move(m_deltaTime);
            player.lookAt(m_camPos, m_viewMatrix);

            m_defaultShader.setMatrix("mvpMatrix", m_projectionMatrix * m_viewMatrix * player.transform.getModelMatrix());
            m_defaultShader.setMatrix("modelMatrix", player.transform.getModelMatrix());
            m_defaultShader.setVector("lightPos", m_light.position);
            m_defaultShader.setVector("camPos", m_camPos);
            m_defaultShader.setMatrix("lightMVP", m_projectionMatrix * m_light.viewMatrix);
            player.bindTexture(2, 3);
            player.draw(m_defaultShader);
            
            m_defaultShader.setMatrix("mvpMatrix", m_projectionMatrix * m_viewMatrix * floor.transform.getModelMatrix());
            m_defaultShader.setMatrix("modelMatrix", floor.transform.getModelMatrix());
            m_defaultShader.setVector("lightPos", m_light.position);
            m_defaultShader.setVector("camPos", m_camPos);
            m_defaultShader.setMatrix("lightMVP", m_projectionMatrix * m_light.viewMatrix);
            floor.bindTexture(1, 3);
            floor.draw(m_defaultShader);

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

    Light m_light;

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
