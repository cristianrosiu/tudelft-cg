#version 450 core

// Model/view/projection matrix
layout(location = 0) uniform mat4 mvp;
layout(location = 2) uniform vec3 planePoint;
layout(location = 3) uniform vec3 planePointNormal;

// Per-vertex attributes
layout(location = 0) in vec3 pos; // World-space position
layout(location = 1) in vec3 normal; // World-space normal

// Data to pass to fragment shader
out vec3 fragPos;
out vec3 fragNormal;

out vec3 color;

#define EPSILON 0.1f

void main() {

    vec3 v = pos - planePoint;
    float dist = dot(v, normalize(planePointNormal));
    vec3 projectedPoint = pos - dist*normalize(planePointNormal);

    if (distance(pos, projectedPoint) < EPSILON)
        color = vec3(0.0, 1.0, 0.0);
    else
        color = vec3(1.0, 0.0, 0.0);
    gl_Position = mvp*vec4(pos, 1.0);

    fragPos = pos;
    fragNormal = normal;
}
