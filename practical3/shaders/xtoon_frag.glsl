#version 430

// Global variables for lighting calculations
//layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D textToon;
layout(location = 3) uniform vec3 lightPos;
layout(location = 4) uniform vec3 cameraPos;
layout(location = 5) uniform vec3 lightColor;
layout(location = 6) uniform vec3 kd;
layout(location = 7) uniform vec3 ks;
layout(location = 8) uniform float shininess;


// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal

vec3 N, L, H, V, ID, IR, R;

float zmin = 1;
float r = 1.2;
float zmax = r*zmin;
float z;
void main()
{
    L = normalize(lightPos - fragPos);
    V = normalize(cameraPos - fragPos);
    H = normalize(L + V);
    N = normalize(fragNormal);
    R = 2 * N * dot(N,L) - L;
    ID = kd * lightColor * dot(N, L);
    IR = ks * pow(dot(N,H), shininess);
    z = length(cameraPos - fragPos);
    // Output the normal as color
    outColor = texture(textToon, vec2(kd.x*dot(N, L) + ks.x*pow(dot(N, H), shininess), (log(z/zmin)/log(zmax/zmin))));
    //outColor = texture(textToon, vec2(0.0, 0.0));

}