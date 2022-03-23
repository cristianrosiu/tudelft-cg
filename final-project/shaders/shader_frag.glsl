#version 450

#define NR_LIGHTS 2 

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 color;
    mat4 viewMatrix;
    float radius;
}; 
  
uniform Material material;
uniform Light lights[NR_LIGHTS];

uniform bool hasTexCoords;
uniform sampler2D colorMap;
uniform sampler2D texShadow[NR_LIGHTS];

uniform mat4 lightMVP;
uniform vec3 lightPos;
uniform vec3 bossLight;
uniform vec3 camPos;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

#define BIAS max(0.001f * (1.0f - dot(normalize(fragNormal), normalize(lights[0].position - fragPos))), 0.0001f)

vec3 calcLight(Light light)
{
    vec3 normal = normalize(fragNormal);
    vec3 lightDir   = normalize(light.position - fragPos);
    vec3 viewDir    = normalize(camPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float radius = 20.f;
    float minLight = 0.01;
    float b = 1.0 / (radius*radius * minLight);
    float dist    = length(light.position - fragPos);
    float att = clamp(1.0 - dist*dist/(light.radius*light.radius), 0.0, 1.0); att *= att;

    // Diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.color * (diff * material.diffuse);

    // Specular
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.color * (spec * material.specular);

    return (diffuse + specular)*att;
} 

float calcShadowPCF(vec4 fragLightCoord, int i)
{
    //if (fragLightCoord.z < 0 || fragLightCoord.z > 1.0)
    //    return 0.0;
    const vec2 offset = 1.0 / textureSize(texShadow[i], 0);
    const int pcfCount = 6;
    const float totalTexels = (pcfCount * 2.0 + 1.0)*(pcfCount * 2.0 + 1.0);
   
    float fragLightDepth = fragLightCoord.z; 
    if (fragLightDepth < 0 || fragLightDepth > 1.0)
        return 0.0;
   
    float averageShadow = 0.0;

    for (int y = -pcfCount ; y <= pcfCount ; y++) {
     for (int x = -pcfCount ; x <= pcfCount ; x++) {    
         vec2 shadowMapCoord = fragLightCoord.xy + vec2(x, y) * offset;
         float shadowMapDepth = texture(texShadow[i], shadowMapCoord).r;
         averageShadow += fragLightDepth - BIAS > shadowMapDepth ? 1.0 : 0.0;
     }
    }
    
    averageShadow /= totalTexels;

    return averageShadow;
}

float calcShadowFactor(vec4 fragLightCoord, int i)
{
    // Depth of the fragment with respect to the light
    float fragLightDepth = fragLightCoord.z;

    if (fragLightDepth < 0 || fragLightDepth > 1.0)
        return 0.0;

    // Shadow map coordinate corresponding to this fragment
    vec2 shadowMapCoord = fragLightCoord.xy;

    // Shadow map value from the corresponding shadow map position
    float shadowMapDepth = texture(texShadow[i], shadowMapCoord).r;
    
    float shadow = fragLightDepth - BIAS > shadowMapDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 lighting = vec3(0.f);
    for (int i = 0; i < NR_LIGHTS; i++)
    {
        vec4 fragLightCoord = lights[i].viewMatrix * vec4(fragPos, 1.0);

        // Divide by w because fragLightCoord are homogeneous coordinates
        fragLightCoord.xyz /= fragLightCoord.w;
    
        // The resulting value is in NDC space (-1 to +1),
        //  we transform them to texture space (0 to 1).
        fragLightCoord.xyz = fragLightCoord.xyz * 0.5 + 0.5;

        // Shadow map coordinate corresponding to this fragment
        vec2 shadowMapCoord = fragLightCoord.xy;

        float distFromCenter = distance(fragLightCoord.xy, vec2(0.5));

        float intensity = distFromCenter > 0.5 ? 0.0 : (0.5 - distFromCenter)*2;

        float shadow = calcShadowPCF(fragLightCoord, i);
        if (i == 1)
            lighting += calcLight(lights[i]) * (1 - shadow) * pow(intensity, 2.0);
        else
            lighting += calcLight(lights[i]) * (1 - shadow);
    }

    if (hasTexCoords)
        fragColor = vec4(lighting*texture(colorMap, fragTexCoord.xy).rgb, 1.0);
    else
        fragColor = vec4(lighting, 1.0);
    
}
