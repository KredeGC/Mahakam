#ifndef VOLUMETRIC_INCLUDED
#define VOLUMETRIC_INCLUDED

#include "internal/shaders/include/lighting/LightStruct.glsl"
#include "internal/shaders/include/Utility.glsl"

// Mie scaterring approximated with Henyey-Greenstein phase function.
float HenyeyGreensteinScattering(float scattering, float LdotV)
{
    float result = 1.0 - scattering * scattering;
    result /= (4.0 * PI * pow(1.0 + scattering * scattering - (2.0 * scattering) * LdotV, 1.5));
    return result;
}

vec3 VolumetricScattering(Light light, vec3 startPos, vec3 viewDir, vec3 worldNormal, vec3 lightDir, float stepSize, int steps) {
    if (light.volumetric.w >= 1.0 - Epsilon)
        return vec3(0.0);
    
    float LdotV = dot(lightDir, -viewDir);
    float mieScattering = HenyeyGreensteinScattering(light.volumetric.w, LdotV);
    
    float fog = 0.0;
    for (int j = 1; j <= steps; j++) {
        vec3 samplePos = startPos - viewDir * (stepSize * j);
        float sampleAttenuation = PBR_SHADOW_RAW(light, samplePos, worldNormal);
        
        fog += sampleAttenuation;
    }
    
    float scattering = (mieScattering * fog) / steps;
    
    return scattering * light.volumetric.rgb * light.color.rgb;
}

#endif // VOLUMETRIC_INCLUDED