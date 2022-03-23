#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include "glm/gtx/string_cast.hpp"


#ifndef TRANSFORM_H
#define TRANSFORM_H


class Transform
{
    protected:
        //Local space information
        glm::vec3 m_pos = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_eulerRot = { 0.0f, 0.0f, 0.0f }; //In degrees
        glm::vec3 m_scale = { 1.0f, 1.0f, 1.0f };

        //Global space information concatenate in matrix
        glm::mat4 m_modelMatrix = glm::mat4(1.0f);

        //Dirty flag
        bool m_isDirty = true;

    protected:
        glm::mat4 getLocalModelMatrix();
    public:
        void updateModelMatrix();
        void updateModelMatrix(const glm::mat4& parentGlobalModelMatrix);

        void setLocalPosition(const glm::vec3& newPosition);
        void setLocalRotation(const glm::vec3& newRotation);
        void setLocalScale(const glm::vec3& newScale);

        const glm::vec3 getGlobalPosition();
        const glm::vec3& getLocalPosition();
        const glm::vec3& getLocalRotation();
        const glm::vec3& getLocalScale();
        const glm::mat4& getModelMatrix();
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;
        glm::vec3 getBackward() const;
        glm::vec3 getForward() const;
        glm::vec3 getGlobalScale() const;
        bool isDirty() const;
        void markDirty(bool const &value);
};

#endif