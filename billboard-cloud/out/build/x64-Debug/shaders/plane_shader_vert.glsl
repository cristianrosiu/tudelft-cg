#version 450 core

// Model/view/projection matrix
layout(location = 0) uniform mat4 mvp;

// Per-vertex attributes
layout(location = 0) in vec3 pos; // World-space position
layout(location = 1) in vec3 normal; // World-space normal

// Data to pass to fragment shader
out vec3 fragPos;
out vec3 fragNormal;

void main() {
    gl_Position = mvp * vec4(pos, 1.0);

    fragPos = pos;
    fragNormal = normal;
}
