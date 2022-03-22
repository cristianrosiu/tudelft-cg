//#include "Image.h"
#include "include/player.h"
#include "camera.h"
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
#include <stdlib.h> 

DISABLE_WARNINGS_POP()
#include <framework/shader.h>
#include <framework/window.h>
#include <functional>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include "node.h"


struct Light {
    glm::vec3 position;
    glm::vec3 color;
};

class Application {
public:
    Application()
        : m_window("Final Project", glm::ivec2(1024, 1024), OpenGLVersion::GL45),
          m_texture("resources/texture1.png")
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
        m_light.position = glm::vec3(2.f, 10.f, 0.f);
        m_light.color = glm::vec3(0.5f);
        
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
        GPUMesh scene{ "resources/scene.obj" };
        MousePicker picker{ &m_window, m_projectionMatrix };

        float distance = 2.f;
        
        // === Create Shadow Texture ===
        GLuint texShadow;
        const int SHADOWTEX_WIDTH = 2048;
        const int SHADOWTEX_HEIGHT = 2048;
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

        int index = 0;
        createBossTree();
        int dummyInteger = 0;
        while (!m_window.shouldClose()) {
            // if (player.health() <= 0)
            //     onGameOver();
            
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            m_window.updateInput();
            
            // Use ImGui for easy input/output of ints, floats, strings, etc...
            ImGui::Begin("Player Stats");
            ImGui::Text("Player Health: %i", player.health()); // Use C printf fImGui::Begin("Window");ormatting rules (%i is a signed integer)
            ImGui::End();

            ImGui::Begin("Boss Stats");
            ImGui::Text("Boss Health: %i", bossHealth); // Use C printf fImGui::Begin("Window");ormatting rules (%i is a signed integer)
            ImGui::End();

            // Boss logic
            getLastPos(rootArm);
            auto distanceFromPlayer = glm::distance(player.position(), lastPos);
            if (distanceFromPlayer < 3.f)
            {
                srand(time(NULL));
                int chance = rand()%2;
                player.setHealth(-1.f*chance);
            }
            
            if (player.isMoving)
            {
                index++;
                if (index >= 1) index = 0;
            }
            else
            {
                index = 0;
            }
            
            // === Stub code for you to fill in order to render the shadow map ===
            {
                // Bind the off-screen framebuffer
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

                // Clear the shadow map and set needed options
                glClear(GL_DEPTH_BUFFER_BIT);

                glViewport(0, 0, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);

                // Bind the shader
                m_shadowShader.bind();

                // Draw Scene
                m_viewMatrix = glm::lookAt(m_light.position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
                m_modelMatrix = glm::mat4(1.f);
                configureUniforms();
                scene.draw();

                m_modelMatrix = player.modelMatrix();
                configureUniforms();
                player.mesh(index).draw();

                // Unbind the off-screen framebuffer
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            // Draw player
            m_defaultShader.bind();

            // Bind the shadow map to texture slot 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texShadow);
            glUniform1i(6, 0);

            // Set viewport size
            glViewport(0, 0, m_window.getWindowSize().x, m_window.getWindowSize().y);

            //clear scene
            glClearDepth(1.0f);
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);

            // Calculate DeltaTime of current frame
            GLfloat currentFrame = (GLfloat)glfwGetTime();
            m_deltaTime = (currentFrame - lastFrame);
            lastFrame = currentFrame;

            // Update camera position
            m_camPos = player.position() + glm::vec3(0.f, 4.f, 4.f);
            m_viewMatrix = glm::lookAt(m_camPos, player.position(), glm::vec3(0.f, 1.f, 0.f));
            picker.update(m_viewMatrix);

            // Update player movement
            player.move(m_deltaTime);
            player.lookAt(picker.getRayPoint(m_camPos, glm::vec3(0.f), glm::normalize(glm::vec3(0.f, 1.f, 0.f))));

            // Bind Scene Textures
            m_texture.bind(1, GL_TRUE, 3);
            m_modelMatrix = glm::mat4(1.f);
            configureUniforms();
            scene.draw();

            // Bind Player Textures
            player.texture().bind(2, GL_TRUE, 3);
            player.sTexture().bind(3, GL_TRUE, 9);
            m_modelMatrix = player.modelMatrix();
            configureUniforms();
            player.mesh(index).draw();
            
            updateBoss();

            // Processes input and swaps the window buffer
            m_window.swapBuffers();
        }
    }

    // In here you can handle key presses
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyPressed(int key, int mods)
    {
        switch (key)
        {
            case GLFW_KEY_W:
                break;
            case GLFW_KEY_S:
                break;
            case GLFW_KEY_A:
                break;
            case GLFW_KEY_D:
                break;
            default:
                break;
        }
        std::cout << "Key pressed: " << key << std::endl;
    }

    // In here you can handle key releases
    // key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
    // mods - Any modifier keys pressed, like shift or control
    void onKeyReleased(int key, int mods)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            break;
        case GLFW_KEY_S:
            break;
        case GLFW_KEY_A:
            break;
        case GLFW_KEY_D:
            break;
        default:
            break;
        }
    }

    // If the mouse is moved this function will be called with the x, y screen-coordinates of the mouse
    void onMouseMove(const glm::dvec2& cursorPos)
    {

        //std::cout << cursorPos.x << " " << cursorPos.y << "\n";
        //// Now, we only needed to un-project the x,y part, so let's manually set the z,w part to mean "forwards, and not a point". From http://antongerdelan.net/opengl/raycasting.html
    }

    // If one of the mouse buttons is pressed this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseClicked(int button, int mods)
    {
        switch (button)
        {
            case GLFW_MOUSE_BUTTON_1:
                getLastPos(rootArm);
                float  distanceFromPlayer = glm::distance(player.position(), lastPos);
                if (distanceFromPlayer < 3.f)
                    bossHealth -= 10.f;
                break;
        }
        
    }

    // If one of the mouse buttons is released this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/grodup__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseReleased(int button, int mods)
    {
        std::cout << "Released mouse button: " << button << std::endl;
    }

    void configureUniforms()
    {
        const glm::mat4 lightMVP = m_projectionMatrix * glm::lookAt(m_light.position, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(lightMVP));

        const glm::mat4 mvpMatrix = m_projectionMatrix * m_viewMatrix * m_modelMatrix;
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
        
        glUniform3fv(7, 1, glm::value_ptr(m_light.position));
        glUniform3fv(8, 1, glm::value_ptr(m_camPos));
    }


    void createBossTree()
    {
        rootArm->translationMatrix = glm::translate(rootArm->translationMatrix, glm::vec3(0.f, 0.f, -1.f));
        rootArm->mesh = GPUMesh("resources/sphere.obj");

        struct Node* arm2 = new struct Node();
        arm2->translationMatrix = glm::translate(arm2->translationMatrix, glm::vec3(0.f, 0.f, -1.f));
        arm2->mesh = GPUMesh("resources/sphere.obj");

        struct Node* arm3 = new struct Node();
        arm3->translationMatrix = glm::translate(arm3->translationMatrix, glm::vec3(0.f, 0.f, -1.f));
        arm3->mesh = GPUMesh("resources/sphere.obj");

        struct Node* arm4 = new struct Node();
        arm4->translationMatrix = glm::translate(arm4->translationMatrix, glm::vec3(0.f, 0.f, -1.f));
        arm4->mesh = GPUMesh("resources/dragon.obj");

        rootArm->next_level = arm2;
        rootArm->next_object = NULL;

        arm2->next_level = arm3;
        arm2->next_object = NULL;

        arm3->next_level = arm4;
        arm3->next_object = NULL;

        arm4->next_level = NULL;
        arm4->next_object = NULL;
    }

    void updateBoss()
    {
        m_bossModelMatrix = glm::mat4(1.f);
        getLastPos(rootArm);
        m_bossModelMatrix = glm::mat4(1.f);
        updateGradient(rootArm, lastPos);
        glm::vec3 targetPos = player.position() - lastPos;
        float distance = glm::distance(player.position(), lastPos); 

        glm::vec3 viewForward = glm::normalize(targetPos);
        float angle = std::atan2f(viewForward.x, viewForward.z);

        if (distance > 5.0f)
        {
            updateJacobian(1);
            glm::vec3 angles = glm::transpose(m_jacobian3x3) * (glm::normalize(targetPos) * 0.1f);
            angleStack.push(angles.x);
            angleStack.push(angles.y);
            angleStack.push(angles.z);
            angleStack.push(0.0f);
            m_bossModelMatrix = glm::mat4(1.f);
        }
        else if (distance > 3.0f && distance < 5.0f) {
            updateJacobian(0);
            glm::vec2 angles = glm::transpose(m_jacobian2x2) * glm::vec2(glm::normalize(targetPos).x * 0.1f, glm::normalize(targetPos).z * 0.1f);
            angleStack.push(0.0f);
            angleStack.push(angles.x);
            angleStack.push(angles.y);
            angleStack.push(angle);
            m_bossModelMatrix = glm::mat4(1.f);
        }
        else {
            angleStack.push(0.0f);
            angleStack.push(0.0f);
            angleStack.push(0.0f);
            angleStack.push(angle);
        }
        m_bossModelMatrix = glm::mat4(1.f);
        render_object(rootArm);
        //rotateHead(rootArm);
    }

    void updateJacobian(int stage)
    {
        glm::vec3 dm0 = m_jacobianStack.front();
        m_jacobianStack.pop();
        //std::cout << glm::to_string(dm0) << "\n";

        glm::vec3 dm1 = m_jacobianStack.front();
        m_jacobianStack.pop();

        glm::vec3 dm2 = m_jacobianStack.front();
        m_jacobianStack.pop();
        //std::cout << glm::to_string(dm1) << "\n\n";
   
        if (stage == 1) {
            m_jacobian3x3 = {
            dm0.x, dm1.x, dm2.x,
            dm0.y, dm1.y, dm2.y,
            dm0.z, dm0.z, dm2.z
            };
        }
        else {
            m_jacobian2x2 = {
            dm1.x, dm2.x,
            dm1.y, dm2.y,
            };
        }
        
    }

    void getLastPos(struct Node* curr_object)
    {
        if (curr_object != NULL) {
            /* Push matrix to stack so we can come back. */
            /* Transformations for this object. */
            m_matrixStack.push(m_bossModelMatrix);

            m_bossModelMatrix = m_bossModelMatrix * curr_object->translationMatrix * curr_object->rotationMatrix;

            if (curr_object->next_level == NULL)
                lastPos =  m_bossModelMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);

            /* Draw object. */
            // TODO: add type of object so you can draw multiples meshes

            /* Render next object lower in the hierarchy. */
            getLastPos(curr_object->next_level);

            /* Restore transformation matrix. */
            m_bossModelMatrix = m_matrixStack.top();
            m_matrixStack.pop();

            /* Render next object at same level in the hierarchy. */
            getLastPos(curr_object->next_object);
        }
    }

    void rotateHead(struct Node* curr_object)
    {
        if (curr_object != NULL) {
            /* Push matrix to stack so we can come back. */
            /* Transformations for this object. */
            m_matrixStack.push(m_bossModelMatrix);
    

            if (curr_object->next_level == NULL) {
                // glm::vec3 viewForward = glm::normalize(player.position() - lastPos);
                // float angle = std::atan2f(viewForward.z, viewForward.x);
 
                // curr_object->rotationMatrix = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f, 1.f, 0.f));
                curr_object->translationMatrix = glm::translate(curr_object->translationMatrix, glm::vec3(0.f, 0.f, -1.f));
            }

            m_bossModelMatrix = m_bossModelMatrix * curr_object->translationMatrix * curr_object->rotationMatrix;
            
            if (curr_object->next_level == NULL)
                lastPos = m_bossModelMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);

            /* Draw object. */
            // TODO: add type of object so you can draw multiples meshes

            /* Render next object lower in the hierarchy. */
            getLastPos(curr_object->next_level);

            /* Restore transformation matrix. */
            m_bossModelMatrix = m_matrixStack.top();
            m_matrixStack.pop();

            /* Render next object at same level in the hierarchy. */
            getLastPos(curr_object->next_object);

        }
    }

    void updateGradient(struct Node* curr_object, glm::vec3 lastPos)
    {
        if (curr_object != NULL) {
            /* Push matrix to stack so we can come back. */
            /* Transformations for this object. */
            m_matrixStack.push(m_bossModelMatrix);

            m_bossModelMatrix = m_bossModelMatrix * curr_object->translationMatrix * curr_object->rotationMatrix;

            glm::vec4 currentPos = m_bossModelMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f);
            glm::vec3 targetPos = lastPos - glm::vec3(currentPos);
            
            if(curr_object->next_level != NULL)
                m_jacobianStack.push(glm::cross(glm::vec3(m_bossModelMatrix*glm::vec4(glm::vec3(0.f, 1.f, 0.f),1.0f)), targetPos));

            /* Draw object. */
            // TODO: add type of object so you can draw multiples meshes

            /* Render next object lower in the hierarchy. */
            updateGradient(curr_object->next_level, lastPos);

            /* Restore transformation matrix. */
            m_bossModelMatrix = m_matrixStack.top();
            m_matrixStack.pop();

            /* Render next object at same level in the hierarchy. */
            updateGradient(curr_object->next_object, lastPos);
        }
    }


    /* Render model by recursively traversing the tree (or DAG) structure. */
    void render_object(struct Node* curr_object)
    {
        if (curr_object != NULL) {
            /* Push matrix to stack so we can come back. */
            /* Transformations for this object. */
            m_matrixStack.push(m_bossModelMatrix);

            if (curr_object->next_level != NULL) {
                curr_object->rotationMatrix = glm::rotate(curr_object->rotationMatrix, angleStack.front(), glm::vec3(0.f, 1.f, 0.f));
                m_bossModelMatrix = m_bossModelMatrix * curr_object->translationMatrix * curr_object->rotationMatrix;
                m_modelMatrix = m_bossModelMatrix;
            }
                
            else {
                curr_object->rotationMatrix = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(glm::mat4(1.f), angleStack.front(), glm::vec3(0.f, 1.f, 0.f));
                m_bossModelMatrix = m_bossModelMatrix * curr_object->translationMatrix * curr_object->rotationMatrix;
                m_modelMatrix = glm::translate(glm::mat4(1.f), lastPos) * glm::rotate(glm::mat4(1.f), angleStack.front() + glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
            }
               
            angleStack.pop();
            /*else {
                glm::vec3 lastPos = m_bossModelMatrix * curr_object->translationMatrix * curr_object->rotationMatrix * glm::vec4(glm::vec3(0.f), 1.f);
                glm::vec3 viewForward = glm::normalize(player.position() - lastPos);
                float angle = std::atan2f(viewForward.z, viewForward.x);

                curr_object->rotationMatrix = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(glm::mat4(1.f), angle, glm::vec3(0.f, 1.f, 0.f));
            }*/
            //m_bossModelMatrix = m_bossModelMatrix * curr_object->translationMatrix * curr_object->rotationMatrix;

            /* Draw object. */
            // TODO: add type of object so you can draw multiples meshes
            //m_modelMatrix = m_bossModelMatrix;
            switch (curr_object->type)
            {
                case NodeType::BODY:

                    break;
            }
            configureUniforms();
            m_arm.draw();

            /* Render next object lower in the hierarchy. */
            render_object(curr_object->next_level);

            /* Restore transformation matrix. */
            m_bossModelMatrix = m_matrixStack.top();
            m_matrixStack.pop();

            /* Render next object at same level in the hierarchy. */
            render_object(curr_object->next_object);
        }
    }
private:
    Window m_window;

    // Shader for default rendering and for depth rendering
    Shader m_defaultShader;
    Shader m_defaultShader2;
    Shader m_shadowShader;

    Light m_light;
    Light m_coneLight;

    GLfloat m_deltaTime;

    Player player{ &m_window, "steve", 100, "resources/Sci_fi_basecolor.jpg", "resources/Sci_fi_metallic.jpg", "resources/animation", glm::vec3(0.0f), glm::vec3(0.0f) };
    GPUMesh m_arm{ "resources/sphere.obj" };

    Texture m_texture;

    struct Node* rootArm = new struct Node;

    int bossHealth = 100;

    bool m_gameOver {false};

    // Projection and view matrices for you to fill in and use
    glm::mat4 m_projectionMatrix = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 30.0f);
    glm::mat4 m_viewMatrix = glm::lookAt(glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec3(0, 1, 0));
    glm::mat4 m_modelMatrix { 1.0f };
    glm::mat3x3 m_jacobian3x3{ 1.f };
    glm::mat2x2 m_jacobian2x2{ 1.f };

    glm::mat4 m_bossModelMatrix{ 1.0f };
    glm::vec3 lastPos{ 0.f };
    std::stack<glm::mat4> m_matrixStack;
    std::queue<glm::vec3> m_jacobianStack;
    std::queue<float> angleStack;

    glm::vec3 m_camPos{ 0.f };
};

int main()
{
    Application app;
    app.update();

    return 0;
}
