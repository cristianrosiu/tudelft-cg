#pragma once
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/vec3.hpp>
DISABLE_WARNINGS_POP()
#include <exception>
#include <filesystem>
#include <framework/opengl_includes.h>

static enum TextureType { DIFFUSE, SPECULAR };

struct ImageLoadingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

#ifndef TEXTURE_H
#define TEXTURE_H

class Texture {
public:
    Texture(std::filesystem::path filePath);
    Texture(std::filesystem::path filePath, TextureType type);
    Texture(const Texture&) = delete;
    Texture(Texture&&);
    ~Texture();

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = default;

    GLuint const getTextureID() const;
    TextureType const getType() const;

    //void bind(GLint textureSlot, GLint active);

private:
    static constexpr GLuint INVALID = 0xFFFFFFFF;
    GLuint m_texture{ INVALID };
    TextureType m_type{ TextureType::DIFFUSE };
};

#endif
