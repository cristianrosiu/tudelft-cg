#version 450 core

// Model/view/projection matrix
layout(location = 0) uniform mat4 mvp;

// VALID set plane vertex information
layout(location = 2) uniform vec3 planePoint;
layout(location = 3) uniform vec3 planePointNormal;

// MISS set plane vertex information
layout(location = 6) uniform vec3 planePoint2;

// Per-vertex attributes
layout(location = 0) in vec3 pos; // World-space position
layout(location = 1) in vec3 normal; // World-space normal

// Booleans
layout(location = 4) uniform int project;
layout(location = 9) uniform int projectMiss;
layout(location = 7) uniform int plane2Active;

layout(location = 5) uniform float EPSILON;

// Position of light for calculation of lambertian shading
layout(location = 8) uniform vec3 lightPos;

// Data to pass to fragment shader
out vec3 fragPos;
out vec3 fragNormal;

out vec4 color;

void main() {

    // Calculate the orthographic projection of vertex onto the VALID plane
    vec3 v = pos - planePoint;
    float dist = dot(v, normalize(planePointNormal));
    vec3 projectedPoint = pos - dist*normalize(planePointNormal);

    // Calculate the orthographic projection of vertex onto the MISS plane
    vec3 v2 = pos - planePoint2;
    float dist2 = dot(v2, normalize(planePointNormal));
    vec3 projectedPoint2 = pos - dist2*normalize(planePointNormal);
    
    // Calculate the VALID and MISS sets of vertices and color them respectively
    if (distance(pos, projectedPoint) < EPSILON && projectMiss == 0) {
        color = vec4(0.0, 1.0, 0.0, 1.0);

        if (project == 1) {
            gl_Position = mvp*vec4(projectedPoint, 1.0);
            fragPos = projectedPoint;
        } else {
            gl_Position = mvp*vec4(pos, 1.0);
            fragPos = pos;
        }  
    }  
    else  if (distance(pos, projectedPoint2) < EPSILON &&  plane2Active == 1) {
        color = vec4(1.0, 0.0, 0.0, 1.0);
         if (projectMiss == 1) {
            gl_Position = mvp*vec4(projectedPoint2, 1.0);
            fragPos = projectedPoint2;
        }else {
            gl_Position = mvp*vec4(pos, 1.0);
            fragPos = pos;
        }
    }
    else {
        color = vec4(vec3(max(dot(normal, normalize(lightPos - pos)), 0.0)), 0.8);
        gl_Position = mvp*vec4(pos, 1.0);
        fragPos = pos;
    } 
        
    fragNormal = normal;
}
