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
#include <stb/stb_image.h>
#include <map>

#define MAX_PLAYER_HEALTH 100
#define MAX_BOSS_HEALTH 100

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
        initializeScene();
        while (!m_window.shouldClose()) {
            m_window.updateInput();
            showUI();
            updateCutscene(player);

            if (boss.getHealth() <= 50)
                textoonActive = true;

            if(player.getHealth() <= 0 || boss.getHealth() <= 0)
                onGameOver();
            
            if(gameOver)
                continue;

            if (isCutscene && glfwGetTime() < 2.5f)
                activeCamera = 1;
            else
            {
                //Updates
                activeCamera = 0;
                m_cameras[activeCamera].position = player.transform.getLocalPosition() + glm::vec3(0.f, 6.f, -4.f);
                m_cameras[activeCamera].viewMatrix = glm::lookAt(m_cameras[activeCamera].position, player.transform.getLocalPosition(), glm::vec3(0.f, 1.f, 0.f));
                player.update(m_cameras[activeCamera].position, m_cameras[activeCamera].viewMatrix, m_deltaTime);
            }
            boss.updateBoss(m_deltaTime);

            m_lights[1].position = boss.getLastPosition() + glm::vec3(0.f, 3.f, 0.f);
            m_lights[1].viewMatrix = glm::lookAt(m_lights[1].position, m_lights[1].position + boss.getLastForward() - glm::vec3(0.f, 1.f, 0.f) , glm::vec3(0.f, 1.f, 0.f));

            // Calculate DeltaTime of current frame
            GLfloat currentFrame = (GLfloat)glfwGetTime();
            m_deltaTime = glm::abs(currentFrame - lastFrame);
            lastFrame = currentFrame;

            // Compute the shadow map textures
            for (int i = 0; i < 2; i++)
                m_shadowMaps[i].renderShadowMap(m_shadowShader, m_projectionMatrix, m_lights[i], dugeon, player, boss);

            // Set viewport size
            glViewport(0, 0, m_window.getWindowSize().x, m_window.getWindowSize().y);
            glClearDepth(1.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);

            m_defaultShader.bind();

            glUniform1i(2, 0);
            glUniform1i(4, 0);

            configureUniforms();

            // Draw dungeon
            for (int i = 0; i < 2; i++)
                m_defaultShader.setMatrix("lights[" + std::to_string(i) + "].viewMatrix", m_projectionMatrix * m_lights[i].viewMatrix * dugeon.transform.getModelMatrix());
            configureMaterialUniforms(dugeon);
            m_defaultShader.setMatrix("modelMatrix", dugeon.transform.getModelMatrix());
            dugeon.draw(m_defaultShader);

            // Draw Player
            for (int i = 0; i < 2; i++)
                m_defaultShader.setMatrix("lights[" + std::to_string(i) + "].viewMatrix", m_projectionMatrix * m_lights[i].viewMatrix * dugeon.transform.getModelMatrix());
            configureMaterialUniforms(player);
            m_defaultShader.setSampler("texShadow[5]", player.getBaseColorTexture(), 6);
            m_defaultShader.setSampler("texShadow[6]", player.getSpecularTexture(), 7);
            m_defaultShader.setMatrix("modelMatrix", player.transform.getModelMatrix());
            glUniform1i(4, 1);
            //player.bindTexture(2, 3);
            player.draw(m_defaultShader);

            if (textoonActive) {
                m_defaultShader.setSampler("texShadow[2]", m_toonTexture.getTextureID(), 3);
                glActiveTexture(GL_TEXTURE0+3);
                glBindTexture(GL_TEXTURE_2D, m_toonTexture.getTextureID());
                glUniform1i(2, 1);
                glUniform1f(3, boss.getHealth() / 50);
            }  
            glUniform1i(4, 0);
            boss.draw(m_defaultShader);

            // Draw smoke texture flying around 
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);     
           
            smoke1.transform.setLocalPosition( glm::vec3(std::sin(glfwGetTime() * 0.1) * 3, 1.f, 0.f));
            smoke1.updateSelfAndChild();

            smoke2.transform.setLocalPosition(glm::vec3(-std::sin(glfwGetTime() * 0.1) * 3, 2.f, 0.f));
            smoke2.updateSelfAndChild();
           

            std::map<float, GameObject*> sorted;
            float distance1 = m_cameras[activeCamera].position.y - smoke1.transform.getGlobalPosition().y;

            float distance2 = m_cameras[activeCamera].position.y - smoke2.transform.getGlobalPosition().y;
            

            if (distance1 > distance2) {
                m_defaultShader.setSampler("texShadow[3]", smoke1.getRgbaTexture(), 4);
                m_defaultShader.setMatrix("modelMatrix", smoke1.transform.getModelMatrix());
                glUniform1i(5, 1);
                smoke1.draw(m_defaultShader);

                m_defaultShader.setSampler("texShadow[4]", smoke1.getRgbaTexture(), 5);
                m_defaultShader.setMatrix("modelMatrix", smoke2.transform.getModelMatrix());
                glUniform1i(5, 1);
                smoke2.draw(m_defaultShader);
            }
            else {
                m_defaultShader.setSampler("texShadow[4]", smoke1.getRgbaTexture(), 5);
                m_defaultShader.setMatrix("modelMatrix", smoke2.transform.getModelMatrix());
                glUniform1i(5, 1);
                smoke2.draw(m_defaultShader);

                m_defaultShader.setSampler("texShadow[3]", smoke1.getRgbaTexture(), 4);
                m_defaultShader.setMatrix("modelMatrix", smoke1.transform.getModelMatrix());
                glUniform1i(5, 1);
                smoke1.draw(m_defaultShader);
            }

            glUniform1i(5, 0);
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
        if(button == GLFW_MOUSE_BUTTON_1)
        {
            if(glm::distance(player.transform.getLocalPosition(), boss.getLastPosition()) < 10.f)
                boss.setHealth(-5);
        }

    }

    // If one of the mouse buttons is released this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/grodup__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseReleased(int button, int mods)
    {
    }

    void configureMaterialUniforms(GameObject& gameObject)
    {
        m_defaultShader.setVector("material.ambient", gameObject.material.ka);
        m_defaultShader.setVector("material.diffuse", gameObject.material.kd);
        m_defaultShader.setVector("material.specular", gameObject.material.ks);
        m_defaultShader.setFloat("material.shininess", gameObject.material.shininess);
    }

    void configureUniforms()
    {
        // Send the shadow map textures to GPU
        m_defaultShader.setSampler("texShadow[0]", m_shadowMaps[0].getTextureID(), 0);
        m_defaultShader.setSampler("texShadow[1]", m_shadowMaps[1].getTextureID(), 1);
        m_defaultShader.setMatrix("projectionMatrix", m_projectionMatrix);
        m_defaultShader.setMatrix("viewMatrix", m_cameras[activeCamera].viewMatrix);
        m_defaultShader.setVector("camPos", m_cameras[activeCamera].position);

        for (int i = 0; i < 2; i++)
        {
            m_defaultShader.setVector("lights[" + std::to_string(i) + "].position", m_lights[i].position);
            m_defaultShader.setVector("lights[" + std::to_string(i) + "].color", m_lights[i].color);
            m_defaultShader.setFloat("lights[" + std::to_string(i) + "].radius", m_lights[i].radius);
        }
    }

    void showUI()
    {
        ImGui::Begin("Restart Button");
        if (ImGui::Button("Restart"))
            restart();

        ImGui::End();
        // Use ImGui for easy input/output of ints, floats, strings, etc...
        ImGui::Begin("Player Stats");
        ImGui::Text("Player Health: %i", player.getHealth()); // Use C printf fImGui::Begin("Window");ormatting rules (%i is a signed integer)
        ImGui::End();

        ImGui::Begin("Boss Stats");
        ImGui::Text("Boss Health: %i", boss.getHealth()); // Use C printf fImGui::Begin("Window");ormatting rules (%i is a signed integer)
        ImGui::End();
    }

    void restart()
    {
        isCutscene = false;
        gameOver = false;
        textoonActive = false;

        player.setHealth(MAX_PLAYER_HEALTH - player.getHealth());        
        player.transform.setLocalPosition(glm::vec3(0.f, 0.f, -13.5f));
        player.updateSelfAndChild();

        boss.setHealth(MAX_BOSS_HEALTH - boss.getHealth());
    }

    void initializeScene()
    {
        player.material.ka = glm::vec3(0.15f);
        player.material.kd = glm::vec3(0.8f);
        player.material.ks = glm::vec3(0.5f);
        player.material.shininess = 10.f;

        player.transform.setLocalPosition(glm::vec3(0.f, 0.f, -13.5f));
        player.updateSelfAndChild();

        // Main light
        m_lights[0].position = glm::vec3(-10.f, 5.f, 0.f);
        m_lights[0].color = glm::vec3(0.5f, 0.5f, 0.5f);
        m_lights[0].radius = 30.f;
        m_lights[0].viewMatrix = glm::lookAt(m_lights[0].position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

        // Boss light
        m_lights[1].color = glm::vec3(0.f, 0.f, 1.f);
        m_lights[1].radius = 40.f;

        // Camera
        m_cameras[1].position = glm::vec3(3.f, 10.f, -5.f);
        m_cameras[1].viewMatrix = glm::lookAt(m_cameras[1].position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

        dugeon.material.ka = glm::vec3(0.15f);
        dugeon.material.kd = glm::vec3(0.8f);
        dugeon.material.ks = glm::vec3(0.2f);
        dugeon.material.shininess = 0.f;
    }

    void onGameOver()
    {
        gameOver = true;
    }

    void updateCutscene(Player &player)
    {
        if (isCutscene)
            return;

        glm::vec3 sceneTrigger(0.f, 0.f, -8.f);

        if (glm::distance(sceneTrigger, player.transform.getGlobalPosition()) < 2.f)
        {
            glfwSetTime(0.f);
            isCutscene = true;
        }
    }

private:
    Window m_window;

    // Shader for default rendering and for depth rendering
    Shader m_defaultShader;
    Shader m_shadowShader;

    Texture m_toonTexture{"resources/toon_map.png"};
    Texture m_specularTexture{ "resources/metal_specular.jpg" };


    ShadowMap m_shadowMaps[2]{ ShadowMap{glm::uvec2(4096)}, ShadowMap{glm::uvec2(4096)} };
    Light m_lights[2];
    Camera m_cameras[2];

    int activeCamera{ 0 };
    bool textoonActive = false;

    bool isCutscene = false;
    bool gameOver = false;

    float tranlateSmoke = 0.f;

    GLfloat m_deltaTime;

    // Projection and view matrices for you to fill in and use
    glm::mat4 m_projectionMatrix = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 30.0f);
    glm::mat4 m_viewMatrix = glm::lookAt(glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec3(0, 1, 0));
    glm::mat4 m_modelMatrix { 1.0f };

    Player player{ "./resources/animation_run", &m_window, m_projectionMatrix, "resources/player-basecolor.jpg", "resources/player-specular.jpg" };
    Boss boss{ std::vector<std::filesystem::path>{"./resources/boss/bodySmall", "./resources/boss/bodyMedium", "./resources/boss/bodyLarge", "./resources/boss/head"}, &player};
    GameObject dugeon{ "./resources/dungeon" };
    GameObject smoke1 { "./resources/floor" , "resources/smoke.png" };
    GameObject smoke2 { "./resources/floor" , "resources/smoke2.png" };
};

int main()
{
    Application app;
    app.update();

    return 0;
}
