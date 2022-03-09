// Disable compiler warnings in third-party code (which we cannot change).
#include <framework/disable_all_warnings.h>
#include <framework/opengl_includes.h>
DISABLE_WARNINGS_PUSH()
// Include glad before glfw3
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
//#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
DISABLE_WARNINGS_POP()
#include <algorithm>
#include <cassert>
#include <cstdlib> // EXIT_FAILURE
#include <framework/mesh.h>
#include <framework/shader.h>
#include <framework/trackball.h>
#include <framework/window.h>
#include <iostream>
#include <numeric>
#include <optional>
#include <span>
#include <vector>

// Configuration
const int WIDTH = 800;
const int HEIGHT = 800;

bool debug = false;
bool diffuseLighting = false;
bool phongSpecularLighting = false;
bool blinnPhongSpecularLighting = false;
bool toonLightingDiffuse = false;
bool toonLightingSpecular = false;
bool toonxLighting = false;

enum class LightPlacementValue {
    Sphere = 0,
    Shadow = 1,
    Specular = 2
};
LightPlacementValue interfaceLightPlacement { LightPlacementValue::Sphere };

struct Light {
    glm::vec3 position;
    glm::vec3 color;
};
std::vector lights { Light { glm::vec3(-6, -1, 0), glm::vec3(1) } };
size_t selectedLight = 0;

static glm::vec3 userInteractionSphere(const glm::vec3& selectedPos, const glm::vec3& camPos)
{
    //RETURN the new light position, defined as follows.
    //selectedPos is a location on the mesh. Use this location to place the light source to cover the location as seen from camPos.
    //Further, the light should be at a distance of 1.5 from the origin of the scene - in other words, located on a sphere of radius 1.5 around the origin.
    //glm::vec3 helper = 1.5f * glm::normalize(camPos - selectedPos);

    return 1.5f * glm::normalize(camPos  - selectedPos);
}

static glm::vec3 userInteractionShadow(const glm::vec3& selectedPos, const glm::vec3& selectedNormal, const glm::vec3& lightPos)
{
    //RETURN the new light position such that the light towards the selectedPos is orthgonal to the normal at that location
    //--- in this way, the shading boundary will be exactly at this location.
    //there are several ways to do this, choose one you deem appropriate given the current light position
    //no panic, I will not judge what solution you chose, as long as the above condition is met.
     

    return glm::cross(selectedNormal, glm::vec3(0.0, 0.0, 1.0)) + selectedPos;
}

static glm::vec3 userInteractionSpecular(const glm::vec3& selectedPos, const glm::vec3& selectedNormal, const glm::vec3& lightPos, const glm::vec3& cameraPos)
{
    //RETURN the new light position such that a specularity (highlight) will be located at selectedPos, when viewed from cameraPos and lit from ligthPos.
    //please ensure also that the light is at a distance of 1 from selectedPos! If the camera is on the wrong side of the surface (normal pointing the other way),
    //then just return the original light position.
    //There is only ONE way of doing this!
    if (glm::dot(glm::normalize(cameraPos - selectedPos), glm::normalize(selectedNormal)) < 0) {
        return lightPos;
    }

    return glm::normalize(glm::reflect(glm::normalize(cameraPos - selectedPos), glm::normalize(selectedNormal)));
}

static size_t getClosestVertexIndex(const Mesh& mesh, const glm::vec3& pos);
static std::optional<glm::vec3> getWorldPositionOfPixel(const Trackball&, const glm::vec2& pixel);
static void userInteraction(const glm::vec3& cameraPos, const glm::vec3& selectedPos, const glm::vec3& selectedNormal);
static void printHelp();

// Program entry point. Everything starts here.
int main(int argc, char** argv)
{
    printHelp();

    Window window { "Shading", glm::ivec2(WIDTH, HEIGHT), OpenGLVersion::GL45 };
    Trackball trackball { &window, glm::radians(50.0f) };

    const Mesh mesh = loadMesh(argc == 2 ? argv[1] : "resources/dragon.obj")[0];
    
    struct {
        // Diffuse (Lambert)
        glm::vec3 kd { 0.5f };
        // Specular (Phong/Blinn Phong)
        glm::vec3 ks { 0.25f };
        float shininess = 3.0f;
        // Toon
        int toonDiscretize = 4;
        float toonSpecularThreshold = 0.f;
    } shadingData;

    window.registerKeyCallback([&](int key, int /* scancode */, int action, int /* mods */) {
        if (action != GLFW_RELEASE)
            return;

        const bool shiftPressed = window.isKeyPressed(GLFW_KEY_LEFT_SHIFT) || window.isKeyPressed(GLFW_KEY_RIGHT_SHIFT);

        switch (key) {
        case GLFW_KEY_0: {
            debug = !debug;
            break;
        }
        case GLFW_KEY_1: {
            diffuseLighting = !diffuseLighting;
            break;
        }
        case GLFW_KEY_2: {
            phongSpecularLighting = !phongSpecularLighting;
            break;
        }
        case GLFW_KEY_3: {
            blinnPhongSpecularLighting = !blinnPhongSpecularLighting;
            break;
        }
        case GLFW_KEY_4: {
            toonLightingDiffuse = !toonLightingDiffuse;
            break;
        }
        case GLFW_KEY_5: {
            toonLightingSpecular = !toonLightingSpecular;
            if (toonLightingSpecular)
                toonLightingDiffuse = true;
            break;
        }
        case GLFW_KEY_6: {
            toonxLighting = !toonxLighting;
            break;
        }
        case GLFW_KEY_7: {
            std::cout << "Number keys from 7 on not used." << std::endl;
            return;
        }
        case GLFW_KEY_M: {
            interfaceLightPlacement = static_cast<LightPlacementValue>((static_cast<int>(interfaceLightPlacement) + 1) % 3);
            break;
        }
        case GLFW_KEY_L: {
            if (shiftPressed)
                lights.push_back(Light { trackball.position(), glm::vec3(1) });
            else
                lights[selectedLight].position = trackball.position();
            return;
        }
        case GLFW_KEY_MINUS: {
            if (selectedLight == 0)
                selectedLight = lights.size() - 1;
            else
                --selectedLight;
            return;
        }
        case GLFW_KEY_EQUAL: {
            if (shiftPressed) // '+' pressed (unless you use a weird keyboard layout).
                selectedLight = (selectedLight + 1) % lights.size();
            return;
        }
        case GLFW_KEY_N: {
            lights.clear();
            lights.push_back(Light { glm::vec3(0, 0, 3), glm::vec3(1) });
            selectedLight = 0;
            return;
        }
        case GLFW_KEY_SPACE: {
            const auto optWorldPoint = getWorldPositionOfPixel(trackball, window.getCursorPixel());
            if (optWorldPoint) {
                //std::cout << "World point: (" << optWorldPoint->x << ", " << optWorldPoint->y << ", " << optWorldPoint->z << ")" << std::endl;
                //lights[selectedLight].position = worldPoint;
                const size_t selectedVertexIdx = getClosestVertexIndex(mesh, *optWorldPoint);
                if (selectedVertexIdx != 0xFFFFFFFF) {
                    const Vertex& selectedVertex = mesh.vertices[selectedVertexIdx];
                    userInteraction(trackball.position(), selectedVertex.position, selectedVertex.normal);
                }
            }
            return;
        }

        case GLFW_KEY_T: {
            if (shiftPressed)
                shadingData.toonSpecularThreshold += 0.001f;
            else
                shadingData.toonSpecularThreshold -= 0.001f;
            std::cout << "ToonSpecular: " << shadingData.toonSpecularThreshold << std::endl;
            return;
        }
        case GLFW_KEY_D: {
            if (shiftPressed) {
                ++shadingData.toonDiscretize;
            } else {
                if (--shadingData.toonDiscretize < 1)
                    shadingData.toonDiscretize = 1;
            }
            std::cout << "Toon Discretization levels: " << shadingData.toonDiscretize << std::endl;
            return;
        }
        case GLFW_KEY_R: {
            if (shiftPressed) {
                // Decrease diffuse Kd coeffcient in the red channel by 0.1
                lights[selectedLight].color = lights[selectedLight].color +  glm::vec3(0.1, 0.0, 0.0);
            } else {
                // Increase diffuse Kd coeffcient in the red channel by 0.1
                lights[selectedLight].color = lights[selectedLight].color - glm::vec3(0.1, 0.0, 0.0);
            }
            return;
        }
        case GLFW_KEY_G: {
            if (shiftPressed) {
                // Decrease diffuse Kd coeffcient in the green channel by 0.1
                lights[selectedLight].color = lights[selectedLight].color + glm::vec3(0.0, 0.1, 0.0);
            }
            else {
                // Increase diffuse Kd coeffcient in the green channel by 0.1
                lights[selectedLight].color = lights[selectedLight].color - glm::vec3(0.0, 0.1, 0.0);
            }
            return;
        }
        case GLFW_KEY_B: {
            // Same for blue.
            if (shiftPressed) {
                // Decrease diffuse Kd coeffcient in the blue channel by 0.1
                lights[selectedLight].color = lights[selectedLight].color + glm::vec3(0.0, 0.0, 0.1);
            }
            else {
                // Increase diffuse Kd coeffcient in the blue channel by 0.1
                lights[selectedLight].color = lights[selectedLight].color - glm::vec3(0.0, 0.0, 0.1);
            }
            return;
        }
        default:
            return;
        };

        if (!toonLightingDiffuse && !toonxLighting) {
            std::cout << "REALISTIC SHADING!" << std::endl;
            std::cout << "__________________" << std::endl;
            if (diffuseLighting) {
                std::cout << ("DiffuseLighting ON") << std::endl;
            } else {
                std::cout << ("DiffuseLighting OFF") << std::endl;
            }

            if (phongSpecularLighting) {
                std::cout << "PhongSpecularLighting ON" << std::endl;
                std::cout << "BlinnPhongSpecularLighting IGNORED" << std::endl;
            } else if (blinnPhongSpecularLighting) {
                std::cout << "PhongSpecularLighting OFF" << std::endl;
                std::cout << "BlinnPhongSpecularLighting ON" << std::endl;
            } else {
                std::cout << "PhongSpecularLighting OFF" << std::endl;
                std::cout << "BlinnPhongSpecularLighting OFF" << std::endl;
            }
        } else {
            std::cout << "TOON SHADING!" << std::endl;
            std::cout << "_____________" << std::endl;
            if (toonxLighting) {
                std::cout << "ToonLightingDiffuse IGNORED" << std::endl;
                std::cout << "ToonLightingSpecular IGNORED" << std::endl;
                std::cout << "ToonX ON" << std::endl;
            } else {
                if (toonLightingDiffuse) {
                    std::cout << "ToonLightingDiffuse ON" << std::endl;
                } else {
                    std::cout << "ToonLightingDiffuse OFF" << std::endl;
                }

                if (toonLightingSpecular) {
                    std::cout << "ToonLightingSpecular ON" << std::endl;
                } else {
                    std::cout << "ToonLightingSpecular OFF" << std::endl;
                }
                std::cout << "ToonX OFF" << std::endl;
            }
        }

        switch (interfaceLightPlacement) {
        case LightPlacementValue::Sphere: {
            std::cout << "Interaction: LightPlacementValue::Sphere" << std::endl;
            break;
        }
        case LightPlacementValue::Shadow: {
            std::cout << "Interaction: LightPlacementValue::Shadow" << std::endl;
            break;
        }
        case LightPlacementValue::Specular: {
            std::cout << "Interaction: LightPlacementValue::Specular" << std::endl;
            break;
        }
        };
    });

    const Shader lightShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/light_vertex.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/light_frag.glsl").build();
    const Shader debugShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/vertex.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/debug_frag.glsl").build();
    const Shader lambertShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/vertex.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/lambert_frag.glsl").build();
    const Shader phongShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/vertex.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/phong_frag.glsl").build();
    const Shader blinnPhongShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/vertex.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/blinn_phong_frag.glsl").build();
    const Shader toonDiffuseShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/vertex.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/toon_diffuse_frag.glsl").build();
    const Shader toonSpecularShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/vertex.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/toon_specular_frag.glsl").build();
    const Shader xToonShader = ShaderBuilder().addStage(GL_VERTEX_SHADER, "shaders/vertex.glsl").addStage(GL_FRAGMENT_SHADER, "shaders/xtoon_frag.glsl").build();

    // Create Vertex Buffer Object and Index Buffer Objects.
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, static_cast<GLsizeiptr>(mesh.vertices.size() * sizeof(Vertex)), mesh.vertices.data(), 0);

    GLuint ibo;
    glCreateBuffers(1, &ibo);
    glNamedBufferStorage(ibo, static_cast<GLsizeiptr>(mesh.triangles.size() * sizeof(decltype(Mesh::triangles)::value_type)), mesh.triangles.data(), 0);

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

    // Load image from disk to CPU memory.
    int width, height, sourceNumChannels; // Number of channels in source image. pixels will always be the requested number of channels (3).
    stbi_uc* pixels = stbi_load("resources/toon_map.png", &width, &height, &sourceNumChannels, STBI_rgb);

    // Create a texture on the GPU with 3 channels with 8 bits each.
    GLuint texToon;
    glCreateTextures(GL_TEXTURE_2D, 1, &texToon);
    glTextureStorage2D(texToon, 1, GL_RGB8, width, height);

    // Upload pixels into the GPU texture.
    glTextureSubImage2D(texToon, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    // Free the CPU memory after we copied the image to the GPU.
    stbi_image_free(pixels);

    // Set behavior for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(texToon, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texToon, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(texToon, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texToon, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Enable depth testing.
    glEnable(GL_DEPTH_TEST);

    // Main loop.
    while (!window.shouldClose()) {
        window.updateInput();

        // Clear the framebuffer to black and depth to maximum value (ranges from [-1.0 to +1.0]).
        glViewport(0, 0, window.getWindowSize().x, window.getWindowSize().y);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set model/view/projection matrix.
        const glm::vec3 cameraPos = trackball.position();
        const glm::mat4 model { 1.0f };
        const glm::mat4 view = trackball.viewMatrix();
        const glm::mat4 projection = trackball.projectionMatrix();
        const glm::mat4 mvp = projection * view * model;

        bool renderedSomething = false;
        auto render = [&]() {
            renderedSomething = true;

            // Set the model/view/projection matrix that is used to transform the vertices in the vertex shader.
            glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));

            // Bind vertex data.
            glBindVertexArray(vao);

            // Execute draw command.
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.triangles.size()) * 3, GL_UNSIGNED_INT, nullptr);
        };

        if (!debug) {
            // Draw mesh into depth buffer but disable color writes.
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LEQUAL);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            debugShader.bind();
            render();

            // Draw the mesh again for each light / shading model.
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Enable color writes.
            glDepthMask(GL_FALSE); // Disable depth writes.
            glDepthFunc(GL_EQUAL); // Only draw a pixel if it's depth matches the value stored in the depth buffer.
            glEnable(GL_BLEND); // Enable blending.
            glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending.

            for (const Light& light : lights) {
                renderedSomething = false;
                if (!renderedSomething) {
                    if (toonxLighting) {
                        xToonShader.bind();

                        // === SET YOUR X-TOON UNIFORMS HERE ===
                        // Values that you may want to pass to the shader are stored in light, shadingData and cameraPos and texToon.
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, texToon);
                        glUniform1i(2, 0); // Change 2 to the uniform index that you want to use.
                        glUniform3fv(3, 1, glm::value_ptr(light.position));
                        glUniform3fv(4, 1, glm::value_ptr(cameraPos));
                        glUniform3fv(5, 1, glm::value_ptr(light.color));
                        glUniform3fv(6, 1, glm::value_ptr(shadingData.kd));
                        glUniform3fv(7, 1, glm::value_ptr(shadingData.ks));
                        glUniform1f(8, shadingData.shininess);
                        render();
                    } else {
                        if (toonLightingDiffuse) {
                            toonDiffuseShader.bind();

                            // === SET YOUR DIFFUSE TOON UNIFORMS HERE ===
                            // Values that you may want to pass to the shader are stored in light, shadingData.
                            glUniform3fv(1, 1, glm::value_ptr(light.position));
                            glUniform3fv(2, 1, glm::value_ptr(light.color));
                            glUniform3fv(3, 1, glm::value_ptr(shadingData.kd));
                            glUniform1i(4, shadingData.toonDiscretize);
                            render();
                        }
                        if (toonLightingSpecular) {
                            toonSpecularShader.bind();

                            // === SET YOUR SPECULAR TOON UNIFORMS HERE ===
                            // Values that you may want to pass to the shader are stored in light, shadingData and cameraPos.
                            glUniform3fv(1, 1, glm::value_ptr(light.position));
                            glUniform3fv(2, 1, glm::value_ptr(shadingData.ks));
                            glUniform1f(3, shadingData.toonSpecularThreshold);
                            glUniform1f(4, shadingData.shininess);

                            render();
                        }
                    }
                }
                if (!renderedSomething) {
                    if (diffuseLighting) {
                        lambertShader.bind();
                        // === SET YOUR LAMBERT UNIFORMS HERE ===
                        // Values that you may want to pass to the shader include light.position, light.color and shadingData.kd.
                        
                        glUniform3fv(1, 1, glm::value_ptr(light.position));
                        glUniform3fv(2, 1, glm::value_ptr(light.color));
                        glUniform3fv(3, 1, glm::value_ptr(shadingData.kd));
                        render();
                    }
                    if (phongSpecularLighting || blinnPhongSpecularLighting) {
                        if (phongSpecularLighting)
                            phongShader.bind();
                        else
                            blinnPhongShader.bind();

                            // === SET YOUR PHONG/BLINN PHONG UNIFORMS HERE ===
                            // Values that you may want to pass to the shader are stored in light, shadingData and cameraPos.
                            glUniform3fv(1, 1, glm::value_ptr(light.position));
                            glUniform3fv(2, 1, glm::value_ptr(light.color));
                            glUniform3fv(3, 1, glm::value_ptr(shadingData.kd));
                            glUniform3fv(4, 1, glm::value_ptr(shadingData.ks));
                            glUniform3fv(5, 1, glm::value_ptr(cameraPos));
                            glUniform1f(6, shadingData.shininess);


                        render();
                    }
                }
            }

            // Restore default depth test settings and disable blending.
            glDepthFunc(GL_LEQUAL);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
        if (!renderedSomething) {
            debugShader.bind();
            //glUniform3fv(1, 1, glm::value_ptr(cameraPos)); // viewPos.
            render();
        }

        // Draw lights as (square) points.
        lightShader.bind();
        {
            const glm::vec4 screenPos = mvp * glm::vec4(lights[selectedLight].position, 1.0f);
            const glm::vec3 color { 1, 1, 0 };

            glPointSize(40.0f);
            glUniform4fv(0, 1, glm::value_ptr(screenPos));
            glUniform3fv(1, 1, glm::value_ptr(color));
            glDrawArrays(GL_POINTS, 0, 1);
        }
        for (const Light& light : lights) {
            const glm::vec4 screenPos = mvp * glm::vec4(light.position, 1.0f);
            //const glm::vec3 color { 1, 0, 0 };

            glPointSize(10.0f);
            glUniform4fv(0, 1, glm::value_ptr(screenPos));
            glUniform3fv(1, 1, glm::value_ptr(light.color));
            glDrawArrays(GL_POINTS, 0, 1);
        }

        // Present result to the screen.
        window.swapBuffers();
    }

    // Be a nice citizen and clean up after yourself.
    glDeleteTextures(1, &texToon);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}

//User interaction - when the user chooses a vertex, you receive its position, normal, its index
//you can use it to NOW modify all global variables, such as the light position, or change material properties etc.
static void userInteraction(const glm::vec3& cameraPos, const glm::vec3& selectedPos, const glm::vec3& selectedNormal)
{
    switch (interfaceLightPlacement) {
    case LightPlacementValue::Sphere: {
        lights[selectedLight].position = userInteractionSphere(selectedPos, cameraPos);
        break;
    }
    case LightPlacementValue::Shadow: {
        lights[selectedLight].position = userInteractionShadow(
            selectedPos, selectedNormal, lights[selectedLight].position);
        break;
    }
    case LightPlacementValue::Specular: {
        lights[selectedLight].position = userInteractionSpecular(
            selectedPos, selectedNormal, lights[selectedLight].position, cameraPos);
        break;
    }
    }
}

static size_t getClosestVertexIndex(const Mesh& mesh, const glm::vec3& pos)
{
    const auto iter = std::min_element(
        std::begin(mesh.vertices), std::end(mesh.vertices),
        [&](const Vertex& lhs, const Vertex& rhs) {
            return glm::length(lhs.position - pos) < glm::length(rhs.position - pos);
        });
    return std::distance(std::begin(mesh.vertices), iter);
}

static std::optional<glm::vec3> getWorldPositionOfPixel(const Trackball& trackball, const glm::vec2& pixel)
{
    float depth;
    glReadPixels(static_cast<int>(pixel.x), static_cast<int>(pixel.y), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

    if (depth == 1.0f) {
        // This is a work around for a bug in GCC:
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80635
        //
        // This bug will emit a warning about a maybe uninitialized value when writing:
        // return {};
        constexpr std::optional<glm::vec3> tmp;
        return tmp;
    }

    // Coordinates convert from pixel space to OpenGL screen space (range from -1 to +1)
    const glm::vec3 win { pixel, depth };

    // View matrix
    const glm::mat4 view = trackball.viewMatrix();
    const glm::mat4 projection = trackball.projectionMatrix();

    const glm::vec4 viewport { 0, 0, WIDTH, HEIGHT };
    return glm::unProject(win, view, projection, viewport);
}

static void printHelp()
{
    Trackball::printHelp();
    std::cout << std::endl;
    std::cout << "Program Usage:" << std::endl;
    std::cout << "0 - activate Debug" << std::endl;
    std::cout << "______________________" << std::endl;
    std::cout << "1 - Diffuse Lighting on" << std::endl;
    std::cout << "2 - Phong Specularities" << std::endl;
    std::cout << "3 - Blinn-Phong Specularities" << std::endl;
    std::cout << "4 - Toon-Shading" << std::endl;
    std::cout << "5 - Toon Specularities" << std::endl;
    std::cout << "6 - X-Toon Shading" << std::endl;
    std::cout << "______________________" << std::endl;
    std::cout << "D - increase Toon discretization steps" << std::endl;
    std::cout << "d - decrease Toon discretization steps" << std::endl;
    std::cout << "T - increase Toon specular threshold" << std::endl;
    std::cout << "t - decrease Toon specular threshold" << std::endl;
    std::cout << "______________________" << std::endl;
    std::cout << "m - Change Interaction Mode - to influence light source" << std::endl;
    std::cout << "l - place the light source at the current camera position" << std::endl;
    std::cout << "L - add an additional light source" << std::endl;
    std::cout << "+ - choose next light source" << std::endl;
    std::cout << "- - choose previous light source" << std::endl;
    std::cout << "N - clear all light sources and reinitialize with one" << std::endl;
    //std::cout << "s - show selected vertices" << std::endl;
    std::cout << "SPACE - call your light placement function with the current mouse position" << std::endl;
}
