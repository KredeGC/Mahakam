#ifndef TOON_LIGHTING_INCLUDED
#define TOON_LIGHTING_INCLUDED

vec3 ToonDirect(vec3 albedo, float metallic, float roughness, vec3 V, vec3 N, vec3 L, vec3 color, float attenuation) {
    const float numBands = 4;
    float NdotL = max(dot(N, L), 0.0);
    
    float bands = ceil(NdotL * numBands) / numBands;
    
    return bands * albedo * color * attenuation;
}

vec3 ToonIndirect(vec3 albedo, float metallic, float roughness, float ao, vec3 V, vec3 N) {
    return albedo * vec3(0.08, 0.085, 0.09);
}

#include "internal/shaders/include/lighting/Shadows.glsl"

#define PBR_DIRECT ToonDirect
#define PBR_INDIRECT ToonIndirect
#define PBR_SHADOW CalculateShadowAttenuation
#define PBR_SHADOW_RAW CalculateShadowAttenuationRaw

#include "internal/shaders/include/lighting/PBR.glsl"

#endif // TOON_LIGHTING_INCLUDED