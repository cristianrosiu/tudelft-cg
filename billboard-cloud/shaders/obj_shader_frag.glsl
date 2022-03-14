#version 450 core

// Global variables for lighting calculations.
layout(location = 1) uniform vec3 viewPos;

// Output for on-screen color.
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader.
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal

// Booleans
layout(location = 4) uniform int project;
layout(location = 9) uniform int projectMiss;

// Normal of point
in vec3 planePointNormal;

in vec4 color;

void main()
{
    // Discard all points that are not in MISS or VALID sets when projecting points
    if (color.a != 1.0 && (project == 1 || projectMiss == 1))
        discard;
    
    outColor = color;
}