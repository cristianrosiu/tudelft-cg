#include "texture.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()
#include <framework/image.h>
#include <stb/stb_image.h>
#include <iostream>


Texture::Texture(std::filesystem::path filePath)
    : Texture(filePath, TextureType::DIFFUSE)
{}

Texture::Texture(std::filesystem::path filePath, TextureType type)
{
    // Load image from disk to CPU memory.
    // Image class is defined in <framework/image.h>
    // Create Texture
    int texChannels;
    stbi_uc* pixels = stbi_load(filePath.string().c_str(), &m_width, &m_height, &texChannels, 3);

    // Create a texture on the GPU with 3 channels with 8 bits each.
    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);
    glTextureStorage2D(m_texture, 1, GL_RGB8, m_width, m_height);

    // Upload pixels into the GPU texture.
    glTextureSubImage2D(m_texture, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Set behavior for when texture coordinates are outside the [0, 1] range.
    glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_type = type;
}

Texture::Texture(std::filesystem::path filePath, bool IsRGBA)
{
 
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_uc* pixels = stbi_load(filePath.string().c_str(), &width, &height, &nrComponents, 0);
    if (pixels)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        //glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filePath.string().c_str() << std::endl;
    }

   

    //int nrComponents;
    //unsigned char* data = stbi_load(filePath.string().c_str(), &m_width, &m_width, &nrComponents, 0);

    //GLenum format = GL_RGBA;

    ////std::cout << nrComponents << "\n";
    ////if (nrComponents == 1)
    ////    format = GL_RED;
    ////else if (nrComponents == 3)
    ////    format = GL_RGB;
    ////else if (nrComponents == 4)
    ////    format = GL_RGBA;

    ////glBindTexture(GL_TEXTURE_2D, textureID);
    //glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_width, 0, format, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //int texChannels;
    //stbi_uc* pixels = stbi_load(filePath.string().c_str(), &m_width, &m_height, &texChannels, 3);

    //// Create a texture on the GPU with 3 channels with 8 bits each.
    //glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);
    //glTextureStorage2D(m_texture, 1, GL_RGB32, m_width, m_height);

    //// Upload pixels into the GPU texture.
    //glTextureSubImage2D(m_texture, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    //// Set behavior for when texture coordinates are outside the [0, 1] range.
    //glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //// Set interpolation for texture sampling (GL_NEAREST for no interpolation).
    //glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
