#version 430

// Global variables for lighting calculations
//layout(location = 1) uniform vec3 viewPos;
layout(location = 1) uniform vec3 lightPos;
layout(location = 2) uniform vec3 ks;
layout(location = 3) uniform float toonSpecularThreshold;
layout(location = 4) uniform float shininess;

// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal

vec3 N, L, H, V;

void main()
{
    L = normalize(lightPos - fragPos);
    H = normalize(L + V);
    N = normalize(fragNormal);
    float IR;

    IR = ks.x * pow(dot(N,H), shininess);
  
    // Output the normal as color
    if (IR > toonSpecularThreshold)
        outColor = vec4(1.0, 1.0, 1.0, 1.0);
    else
        outColor = vec4(0.0, 0.0, 0.0, 1.0);


    //outColor = vec4(kd * lightColor * dot(fragNormal, lightPos - fragPos), 1.0);
}