#version 450 core

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
// Normals should be transformed differently than positions:
// https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
//layout(location = 3) uniform mat3 normalModelMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main()
{
    mat4 mvpMatrix = projectionMatrix*viewMatrix*modelMatrix;
    gl_Position = mvpMatrix * vec4(position, 1);
    
    fragPos = (modelMatrix * vec4(position, 1)).xyz;
    fragNormal = mat3(inverse(transpose(modelMatrix))) * normal;
    fragTexCoord = texCoord;
}
