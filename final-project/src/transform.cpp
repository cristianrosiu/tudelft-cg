#include "transform.h"
#include <iostream>

glm::mat4 Transform::getLocalModelMatrix()
{
    const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
                                 glm::radians(m_eulerRot.x),
                                 glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
                                 glm::radians(m_eulerRot.y),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
                                 glm::radians(m_eulerRot.z),
                                 glm::vec3(0.0f, 0.0f, 1.0f));

    // Y * X * Z
    const glm::mat4 roationMatrix = transformY * transformX * transformZ;

    // translation * rotation * scale (also know as TRS matrix)
    return glm::translate(glm::mat4(1.0f), m_pos) * roationMatrix * glm::scale(glm::mat4(1.0f), m_scale);
}

void Transform::updateModelMatrix()
{
    m_modelMatrix = getLocalModelMatrix();
}

void Transform::updateModelMatrix(const glm::mat4& parentGlobalModelMatrix)
{
    m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
}

void Transform::setLocalPosition(const glm::vec3& newPosition)
{
    m_pos = newPosition;
    m_isDirty = true;
}

void Transform::setLocalRotation(const glm::vec3& newRotation)
{
    m_eulerRot = newRotation;
    m_isDirty = true;
}

void Transform::setLocalScale(const glm::vec3& newScale)
{
    m_scale = newScale;
    m_isDirty = true;
}

const glm::vec3 Transform::getGlobalPosition()
{
    return glm::vec3(m_modelMatrix[3]);
}

const glm::vec3& Transform::getLocalPosition()
{
    return m_pos;
}

const glm::vec3& Transform::getLocalRotation()
{
    return m_eulerRot;
}

const glm::vec3& Transform::getLocalScale()
{
    return m_scale;
}

const glm::mat4& Transform::getModelMatrix()
{
    return m_modelMatrix;
}

glm::vec3 Transform::getRight() const
{
    return m_modelMatrix[0];
}


glm::vec3 Transform::getUp() const
{
    return m_modelMatrix[1];
}

glm::vec3 Transform::getBackward() const
{
    return m_modelMatrix[2];
}

glm::vec3 Transform::getForward() const
{
    return -m_modelMatrix[2];
}

glm::vec3 Transform::getGlobalScale() const
{
    return { glm::length(getRight()), glm::length(getUp()), glm::length(getBackward()) };
}

bool Transform::isDirty() const
{
    return m_isDirty;
}

void Transform::markDirty(bool const& value)
{
    m_isDirty = value;
}
