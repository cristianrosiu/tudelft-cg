#include "shadowmap.h"
#include <iostream>

ShadowMap::ShadowMap(glm::uvec2 const& size)
{
    // === Create Shadow Texture ===
    const int SHADOWTEX_WIDTH = size.x;
    const int SHADOWTEX_HEIGHT = size.y;
    glCreateTextures(GL_TEXTURE_2D, 1, &m_texShadow);
    glTextureStorage2D(m_texShadow, 1, GL_DEPTH_COMPONENT32F, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);

    // Set behaviour for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(m_texShadow, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_texShadow, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(m_texShadow, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(m_texShadow, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // === Create framebuffer for extra texture ===
    glCreateFramebuffers(1, &m_framebuffer);
    glNamedFramebufferTexture(m_framebuffer, GL_DEPTH_ATTACHMENT, m_texShadow, 0);

    m_size = size;
}

void ShadowMap::renderShadowMap(Shader& shader, glm::mat4 const& projectionMatrix, Light light, GameObject &gameObject,...)
{
    va_list valist;
    // Bind the off-screen framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    
    // Clear the shadow map and set needed options
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, m_size.x, m_size.y);

    // Bind the shader
    shader.bind();

    // Draw Scene
    va_start(valist, &gameObject);
        //auto lightMVP = projectionMatrix * light.viewMatrix * gameObject.transform.getModelMatrix();
        shader.setMatrix("projectionMatrix", projectionMatrix);
        shader.setMatrix("viewMatrix", light.viewMatrix);
        shader.setMatrix("modelMatrix", gameObject.transform.getModelMatrix());
        gameObject.draw(shader);
    va_end(valist);
    

    // Unbind the off-screen framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint const ShadowMap::getTextureID() const
{
    return m_texShadow;
}