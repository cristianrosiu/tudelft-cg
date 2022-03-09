#version 430

// Global variables for lighting calculations
//layout(location = 1) uniform vec3 viewPos;
layout(location = 1) uniform vec3 lightPos;
layout(location = 2) uniform vec3 lightColor;
layout(location = 3) uniform vec3 kd;
layout(location = 4) uniform vec3 ks;
layout(location = 5) uniform vec3 cameraPos;
layout(location = 6) uniform float shininess;


// Output for on-screen color
layout(location = 0) out vec4 outColor;

vec3 N, L, R, V, ID, IR;

// Interpolated output data from vertex shader
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal

void main()
{
    N = normalize(fragNormal);
    L = normalize(lightPos - fragPos);
    R = 2 * N * dot(N,L) - L;
    V = normalize(cameraPos - fragPos);


    //ID = kd * lightColor * dot(N, L); 
    IR = ks * pow(dot(R, V), shininess);
    // Output the normal as color
    outColor = vec4(IR, 1.0);
}