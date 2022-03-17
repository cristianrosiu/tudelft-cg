//#include "Image.h"
#include "include/player.h"
#include "camera.h"
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
#include <imgui/imgui.h>
DISABLE_WARNINGS_POP()
#include <framework/shader.h>
#include <framework/window.h>
#include <functional>
#include <iostream>
#include <vector>

struct Light {
    glm::vec3 position;
    glm::vec3 color;
};

class Application {
public:
    Application()
        : m_window("Final Project", glm::ivec2(1024, 1024), OpenGLVersion::GL45),
          m_camera(&m_window, glm::vec3(1.2f, 1.1f, 0.9f), -glm::vec3(1.2f, 1.1f, 0.9f))
    {
        m_window.registerKeyCallback([this](int key, int scancode, int action, int mods) {
            if (action == GLFW_PRESS)
                onKeyPressed(key, mods);
            else if (action == GLFW_RELEASE)
                onKeyReleased(key, mods);
        });
        //m_window.registerMouseMoveCallback(std::bind(&Application::onMouseMove, this, std::placeholders::_1));
        /*m_window.registerMouseButtonCallback([this](int button, int action, int mods) {
            if (action == GLFW_PRESS)
                onMouseClicked(button, mods);
            else if (action == GLFW_RELEASE)
                onMouseReleased(button, mods);
        });*/
        m_light.position = glm::vec3(10.f, 10.f, 0.f);
        m_light.color = glm::vec3(1.f);
        
        try {
            m_defaultShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/shader_frag.glsl").build();
            m_defaultShader2 = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/shader_frag.glsl").build();

            ShaderBuilder shadowBuilder;
            shadowBuilder.addStage(GL_VERTEX_SHADER, "shaders/shader_vert.glsl");
            m_shadowShader = shadowBuilder.build();

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
        glm::vec3 pos = glm::vec3(0.f);

        glm::vec3 m_forward = glm::vec3(1.f, 0.f, 0.f);

        GLfloat lastFrame = (GLfloat)glfwGetTime();

        GPUMesh scene{ "resources/scene.obj" };

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

        while (!m_window.shouldClose()) {
            glfwPollEvents();

            // This is your game loop
            // Put your real-time logic and rendering in here
            m_window.updateInput();

             // Calculate DeltaTime of current frame
            GLfloat currentFrame = (GLfloat)glfwGetTime();
            m_deltaTime = (currentFrame - lastFrame);
            lastFrame = currentFrame;

            player.move(m_deltaTime);
            player.rotate(m_deltaTime);

            {
                // Bind the off-screen framebuffer
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

                // Clear the shadow map and set needed options
                glClearDepth(1.0f);
                glClear(GL_DEPTH_BUFFER_BIT);
                glEnable(GL_DEPTH_TEST);

                // Bind the shader
                m_shadowShader.bind();

                // Set viewport size
                glViewport(0, 0, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);
                
                const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(player.modelMatrix()));
                glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(player.modelMatrix()));
                glUniformMatrix3fv(2, 1, GL_FALSE, glm::value_ptr(normalModelMatrix));

                // .... HERE YOU MUST ADD THE CORRECT UNIFORMS FOR RENDERING THE SHADOW MAP
                const glm::mat4 lightMVP = m_projectionMatrix * m_viewMatrix;
                glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(lightMVP));

                player.mesh().draw();

                // Unbind the off-screen framebuffer
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }


            // Clear the screen
            //glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // ...
            //glEnable(GL_DEPTH_TEST);

            glm::vec3 camPos = player.position() - player.d_forward * 5.f + glm::vec3(0.f, 3.f, 0.f);
            m_viewMatrix = glm::lookAt(camPos, player.position(), glm::vec3(0.f, 1.f, 0.f));

            const glm::mat4 mvpMatrix = m_projectionMatrix * m_viewMatrix * player.modelMatrix();
            // Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
            // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
            const glm::mat3 normalModelMatrix = glm::inverseTranspose(glm::mat3(player.modelMatrix()));

            m_defaultShader.bind();

            glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(player.modelMatrix()));
            glUniformMatrix3fv(2, 1, GL_FALSE, glm::value_ptr(normalModelMatrix));
            // .... HERE YOU MUST ADD THE CORRECT UNIFORMS FOR RENDERING THE SHADOW MAP
            const glm::mat4 lightMVP = m_projectionMatrix * m_viewMatrix;
            glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(lightMVP));
            glUniform3fv(4, 1, glm::value_ptr(m_light.position));
            glUniform3fv(5, 1, glm::value_ptr(m_light.color));
            glUniform3fv(6, 1, glm::value_ptr(glm::vec3(.5f)));
            glUniform3fv(7, 1, glm::value_ptr(camPos));


            player.mesh().draw();


            const glm::mat4 mvpMatrix2 = m_projectionMatrix * m_viewMatrix * glm::mat4(1.f);
            // Normals should be transformed differently than positions (ignoring translations + dealing with scaling):
            // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
            const glm::mat3 normalModelMatrix2 = glm::inverseTranspose(glm::mat3(glm::mat4(1.f)));

            

            m_defaultShader2.bind();
           
            glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(lightMVP));
            glUniform3fv(4, 1, glm::value_ptr(m_light.position));
            glUniform3fv(5, 1, glm::value_ptr(m_light.color));
            glUniform3fv(6, 1, glm::value_ptr(glm::vec3(.5f)));
            glUniform3fv(7, 1, glm::value_ptr(camPos));
            glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvpMatrix2));
            glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
            glUniformMatrix3fv(2, 1, GL_FALSE, glm::value_ptr(normalModelMatrix2));
           
            
            

            scene.draw();

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

        //GLfloat z_cursor;
        //glReadPixels(x_cursor, y_cursor, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z_cursor);

        //// obtain the world coordinates
        //GLdouble x, y, z;
        //gluUnProject(x_cursor, y_cursor, z_cursor, modelview, projection, viewport, &x, &y, &z);

        //float x = (2.0f * cursorPos.x) / m_window.getWindowSize().x - 1.0f;
        //float y = 1.0f - (2.0f * cursorPos.y) / m_window.getWindowSize().y;

        //// 3D Normalised Device Coordinates
        //float z = -1.0f; // the camera looks on the negative z axis
        //glm::vec3 rayNds = glm::vec3(x, y, z);

        //// 4D Homogeneous Clip Coordinates
        //glm::vec4 rayClip = glm::vec4(rayNds, 1.0);

        //// 4D Eye (Camera) Coordinates
        //glm::vec4 rayEye = glm::inverse(m_projectionMatrix) * rayClip;

        //// Now, we only needed to un-project the x,y part, so let's manually set the z,w part to mean "forwards, and not a point". From http://antongerdelan.net/opengl/raycasting.html
        //rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

        //// 4D World Coordinates
        //glm::vec3 rayWorld = glm::vec3(glm::inverse(m_modelMatrix) * rayEye);

        //rayWorld = glm::normalize(rayWorld);

        //std::cout << rayWorld.x << " " << rayWorld.y << " " << rayWorld.z << std::endl;
        //player.rotate(rayWorld);


        //x = (2.0f * x) / m_window.getWindowSize().x - 1.0f;
        //y = 1.0f - (2.0f * y) / m_window.getWindowSize().y;

        /*glm::mat4 invMat = glm::inverse(m_projectionMatrix * m_viewMatrix);
        glm::vec4 near = glm::vec4(2.0f * (x - (m_window.getWindowSize().x/2.0f)) / m_window.getWindowSize().x, -2.0f * (y - m_window.getWindowSize().y / 2.0f) / m_window.getWindowSize().y, -1, 1.0);
        glm::vec4 far = glm::vec4(2.0f * (x - (m_window.getWindowSize().x / 2.0f)) / m_window.getWindowSize().x, -2.0f * (y - m_window.getWindowSize().y / 2.0f) / m_window.getWindowSize().y, 1, 1.0);
        glm::vec4 nearResult = invMat * near;
        glm::vec4 farResult = invMat * far;
        nearResult /= nearResult.w;
        farResult /= farResult.w;
        glm::vec3 dir = glm::vec3(farResult - nearResult);
        glm::vec3 ray_wor = glm::normalize(dir);

        std::cout << ray_wor.x << " " << ray_wor.y << " " << ray_wor.z << std::endl;
        m_viewMatrix = glm::lookAt(player.position(), ray_wor, glm::vec3(0., 1.f, 0.f));*/


        //double x = cursorPos.x;
        //double y = cursorPos.y;

        //x = (2.0f * x) / m_window.getWindowSize().x - 1.0f;
        //y = 1.0f - (2.0f * y) / m_window.getWindowSize().y;

        //glm::vec4 ray_clip = glm::vec4(x, y, -1.0, 1.0);
        //glm::vec4 ray_eye = glm::inverse(m_projectionMatrix) * ray_clip;
        //ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 1.0f);

        //glm::vec3 ray_wor = glm::vec3((glm::inverse(m_viewMatrix) * ray_eye));
        //// don't forget to normalise the vector at some point
        //ray_wor = glm::normalize(ray_wor);

        //float denom = glm::dot(glm::vec3(0.f, 1.f, 0.f), ray_wor);
        //std::cout << (glm::vec3(ray_eye / ray_eye.w) + ray_wor * 20.f).x << (glm::vec3(ray_eye / ray_eye.w) + ray_wor * 20.f).y << (glm::vec3(ray_eye / ray_eye.w) + ray_wor * 20.f).z << "\n";
       
        //player.rotate(glm::vec3(ray_eye / ray_eye.w));
        //
        //
        //if (denom > 1e-6)
        //{
        //    glm::vec3 p0l0 = glm::vec3(0.f) - glm::vec3(ray_eye / ray_eye.w);
        //    float t = glm::dot(p0l0, glm::vec3(0.f, 1.f, 0.f)) / denom;
        //     
        //}

        
        
        //std::cout << "Mouse at position: " << cursorPos.x << " " << cursorPos.y << std::endl;
    }

    // If one of the mouse buttons is pressed this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseClicked(int button, int mods)
    {

        
    }

    // If one of the mouse buttons is released this function will be called
    // button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
    // mods - Any modifier buttons pressed
    void onMouseReleased(int button, int mods)
    {
        std::cout << "Released mouse button: " << button << std::endl;
    }

private:
    Window m_window;

    // Shader for default rendering and for depth rendering
    Shader m_defaultShader;
    Shader m_defaultShader2;
    Shader m_shadowShader;

    Camera m_camera;

    Light m_light;

    GLfloat m_deltaTime;

    Player player{ &m_window, "steve", 100, "resources/checkerboard.png", "resources/dragon.obj", glm::vec3(0.0f), glm::vec3(0.0f) };

    // Projection and view matrices for you to fill in and use
    glm::mat4 m_projectionMatrix = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 30.0f);
    glm::mat4 m_viewMatrix = glm::lookAt(glm::vec3(-1, 1, -1), glm::vec3(0), glm::vec3(0, 1, 0));
    glm::mat4 m_modelMatrix { 1.0f };
};

int main()
{
    Application app;
    app.update();

    return 0;
}
