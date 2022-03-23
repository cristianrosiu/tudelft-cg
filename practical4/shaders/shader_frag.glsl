#version 450 core

// Global variables for lighting calculations.
layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D texShadow;
layout(location = 5) uniform sampler2D texLight;

layout(location = 3) uniform mat4 lightMVP;
layout(location = 4) uniform vec3 lightPos;

// Output for on-screen color.
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader.
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal


vec3 lightDirection = normalize(lightPos - fragPos);

#define BIAS max(0.001f * (1.0f - dot(fragNormal, lightDirection)), 0.0001f)

float calcShadowPCF(vec4 fragLightCoord)
{
   const float imageRes = 1024.0;
   const vec2 offset = 1.0 / textureSize(texShadow, 0);
   const int pcfCount = 6;
   const float totalTexels = (pcfCount * 2.0 + 1.0)*(pcfCount * 2.0 + 1.0);
   
   float fragLightDepth = fragLightCoord.z; 
   if (fragLightDepth < 0 || fragLightDepth > 1.0)
        return 0.0;
   
   float averageShadow = 0.0;

   for (int y = -pcfCount ; y <= pcfCount ; y++) {
    for (int x = -pcfCount ; x <= pcfCount ; x++) {    
        vec2 shadowMapCoord = fragLightCoord.xy + vec2(x, y) * offset;
        float shadowMapDepth = texture(texShadow, shadowMapCoord).r;

        averageShadow += fragLightDepth - BIAS > shadowMapDepth ? 1.0 : 0.0;
    }
   }
    
    averageShadow /= totalTexels;

    return averageShadow;
}

float calcShadowFactor(vec4 fragLightCoord)
{
    // Depth of the fragment with respect to the light
    float fragLightDepth = fragLightCoord.z;

    //if (fragLightDepth < 0 || fragLightDepth > 1.0)
    //    return 0.0;

    // Shadow map coordinate corresponding to this fragment
    vec2 shadowMapCoord = fragLightCoord.xy;

    // Shadow map value from the corresponding shadow map position
    float shadowMapDepth = texture(texShadow, shadowMapCoord).r;
    
    float shadow = fragLightDepth - BIAS > shadowMapDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec4 fragLightCoord = lightMVP * vec4(fragPos, 1.0);

    // Divide by w because fragLightCoord are homogeneous coordinates
    fragLightCoord.xyz /= fragLightCoord.w;
    
    // The resulting value is in NDC space (-1 to +1),
    //  we transform them to texture space (0 to 1).
    fragLightCoord.xyz = fragLightCoord.xyz * 0.5 + 0.5;

    // Output the normal as color
    vec3 lightDir = normalize(lightPos - fragPos);


    // Shadow map coordinate corresponding to this fragment
    vec2 shadowMapCoord = fragLightCoord.xy;

    float distFromCenter = distance(fragLightCoord.xy, vec2(0.5));

    float intensity = distFromCenter > 0.5 ? 0.0 : (0.5 - distFromCenter)*2;

    float lightFactor = calcShadowFactor(fragLightCoord);

    vec3 color = texture(texLight, shadowMapCoord).rgb;
    //outColor = vec4(vec3(max(dot(fragNormal, lightDir)*(1.0 - lightFactor), 0.0)), 1.0);
    outColor = vec4(vec3(max(dot(fragNormal, lightDir), 0.0))*(1.0 - lightFactor)*pow(intensity, 0.1)*color, 1.0);
    
    //outColor = vec4(vec3(fragLightCoord.z), 1);
}