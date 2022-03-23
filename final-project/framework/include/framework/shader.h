#pragma once
#include "disable_all_warnings.h"
#include "opengl_includes.h"
DISABLE_WARNINGS_PUSH()
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
DISABLE_WARNINGS_POP()
#include <exception>
#include <filesystem>
#include <vector>

struct ShaderLoadingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Shader {
public:
    Shader();
    Shader(const Shader&) = delete;
    Shader(Shader&&);
    ~Shader();

    Shader& operator=(Shader&&);

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMatrix(const std::string& name, const glm::mat4& value) const;
    void setMatrix(const std::string& name, const glm::mat3& value) const;
    void setVector(const std::string& name, const glm::vec3& value) const;
    void setVector(const std::string& name, const glm::vec2& value) const;
    void setSampler(const std::string& name, GLuint texture, int const& slot) const;

    void bind() const;

private:
    friend class ShaderBuilder;
    Shader(GLuint program);

private:
    GLuint m_program;
};

class ShaderBuilder {
public:
    ShaderBuilder() = default;
    ShaderBuilder(const ShaderBuilder&) = delete;
    ShaderBuilder(ShaderBuilder&&) = default;
    ~ShaderBuilder();

    ShaderBuilder& addStage(GLuint shaderStage, std::filesystem::path shaderFile);
    Shader build();

private:
    void freeShaders();

private:
    std::vector<GLuint> m_shaders;
};
