#version 430

// Global variables for lighting calculations
//layout(location = 1) uniform vec3 viewPos;
layout(location = 1) uniform vec3 lightPos;
layout(location = 2) uniform vec3 lightColor;
layout(location = 3) uniform vec3 kd;

// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal

vec3 N, L;

void main()
{
    N = normalize(fragNormal);
    L = normalize(lightPos - fragPos);

    // Output the normal as color
    outColor = vec4(kd * lightColor * dot(N, L), 1.0);
}