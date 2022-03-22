#include "texture.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()
#include <framework/image.h>
#include <stb/stb_image.h>


Texture::Texture(std::filesystem::path filePath)
    : Texture(filePath, TextureType::DIFFUSE)
{}

Texture::Texture(std::filesystem::path filePath, TextureType type)
{
    // Load image from disk to CPU memory.
    // Image class is defined in <framework/image.h>
    // Create Texture
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(filePath.string().c_str(), &texWidth, &texHeight, &texChannels, 3);

    // Create a texture on the GPU with 3 channels with 8 bits each.
    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);
    glTextureStorage2D(m_texture, 1, GL_RGB8, texWidth, texHeight);

    // Upload pixels into the GPU texture.
    glTextureSubImage2D(m_texture, 0, 0, 0, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Set behavior for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_type = type;
}


Texture::Texture(Texture&& other)
    : m_texture(other.m_texture)
{
    other.m_texture = INVALID;
}

Texture::~Texture()
{
    if (m_texture != INVALID)
        glDeleteTextures(1, &m_texture);
}

GLuint const Texture::getTextureID() const
{
    return m_texture;
}

TextureType const Texture::getType() const
{
    return m_type;
}

//void Texture::bind(GLint textureSlot, GLint active)
//{
//
//    glActiveTexture(GL_TEXTURE0 + textureSlot);
//    glBindTexture(GL_TEXTURE_2D, m_texture);
//    glUniform1i(3, textureSlot);
//    glUniform1i(4, active);
//}
