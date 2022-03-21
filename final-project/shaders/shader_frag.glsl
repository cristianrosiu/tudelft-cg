#version 450
layout(location = 3) uniform sampler2D colorMap;
layout(location = 4) uniform bool hasTexCoords;
layout(location = 5) uniform mat4 lightMVP;
layout(location = 6) uniform sampler2D texShadow;
layout(location = 7) uniform vec3 lightPos;
layout(location = 8) uniform vec3 camPos;
layout(location = 9) uniform sampler2D sMap;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

vec3 lightDirection = normalize(lightPos - fragPos);
#define BIAS max(0.001f * (1.0f - dot(normalize(fragNormal), lightDirection)), 0.0001f)

float calcShadowPCF(vec4 fragLightCoord)
{
   const float imageRes = 2048.0;
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
    const vec3 normal = normalize(fragNormal);


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

        float shadow = calcShadowPCF(fragLightCoord);

        // Ambient
        vec3 lightColor = vec3(1.0);
        vec3 ambient = 0.05 * lightColor;

        // Diffuse
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * lightColor;

        // Specular
        vec3 viewDir = normalize(camPos - fragPos);
        float spec = 0.0;
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
        vec3 specular = lightColor * (spec * texture(sMap, fragTexCoord).rgb); //spec * lightColor;

        vec3 lighting = ambient + (1.0 - shadow) * pow(intensity, 2.0) * (diffuse + specular);    

        if (hasTexCoords) 
            fragColor = vec4(lighting*texture(colorMap, fragTexCoord.xy).rgb, 1.0);
        else
            fragColor = vec4(lighting, 1.0);
    
}
