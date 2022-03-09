#version 430

// Global variables for lighting calculations
//layout(location = 1) uniform vec3 viewPos;
layout(location = 1) uniform vec3 lightPos;
layout(location = 2) uniform vec3 lightColor;
layout(location = 3) uniform vec3 kd;
layout(location = 4) uniform int toonDiscretize;

// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal

void main()
{
    // Output the normal as color
    float intensity;
    intensity = dot(normalize(fragNormal), normalize(lightPos - fragPos));

    float increments;
    increments = kd.x / toonDiscretize;

    for(int i=0;i<=int(toonDiscretize);++i)
    {
        if (intensity < increments*i && (i == 1 || i == 0))
            outColor = vec4(0.0, 0.0, 0.0, 1.0);
        else if (intensity > increments*i)
            outColor = vec4(increments*i*lightColor, 1.0);
        
        //outColor = vec4(increments*i*lightColor, 1.0);
       

    }

    //outColor = vec4(kd * lightColor * dot(fragNormal, lightPos - fragPos), 1.0);
}
