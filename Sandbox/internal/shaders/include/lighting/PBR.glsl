#ifndef PBR_INCLUDED
#define PBR_INCLUDED

#include "internal/shaders/include/lighting/LightStruct.glsl"

layout(binding = 9, location = 9) uniform sampler2D u_AttenuationLUT;
layout(binding = 10, location = 10) uniform sampler2D u_LightCookie;


// TODO: Move this to a separate pass
// BUG: The material viewer in Erebor shouldn't use volumetric lighting
#define NUM_VOLUMETRIC_STEPS 10
#if defined(DIRECTIONAL) || defined(SPOT)
#include "internal/shaders/include/lighting/Volumetric.glsl"
#endif // DIRECTIONAL || SPOT


vec3 BRDF(vec3 albedo, float metallic, float roughness, float ao, vec3 viewDir, vec3 worldPos, vec3 worldNormal) {
    #if defined(POINT) || defined(SPOT) // Direct lighting
        // Calculate per-light radiance
        Light light = lights[v_InstanceID];
        
        #if defined(POINT)
            vec3 lightVec = light.position.xyz - worldPos;
            
            float distSqr = dot(lightVec, lightVec);
            float rcpRangeSqr = light.color.w;
            
            float normalizedDist = max(distSqr * rcpRangeSqr, 0.00001);
            float attenuation = texture(u_AttenuationLUT, normalizedDist.rr).r;
            
            if (normalizedDist > 1.0) // Find something that saves more performance?
                discard;
            
            vec3 color = light.color.rgb;
        #elif defined(SPOT)
            vec3 lightVec = light.objectToWorld[3].xyz - worldPos;
            
            float distSqr = dot(lightVec, lightVec);
            float rcpRangeSqr = light.color.w;
            
            vec4 uvCookie = light.worldToLight * vec4(worldPos, 1.0);
            // Negative bias because http://aras-p.info/blog/2010/01/07/screenspace-vs-mip-mapping/
            vec3 cookie = texture(u_LightCookie, uvCookie.xy / uvCookie.w * 0.5 + vec2(0.5, 0.5), -8.0).rgb;
            float attenuation = uvCookie.w > 0.0 ? 1.0 : 0.0;
            
            float shadowAttenuation = PBR_SHADOW(light, worldPos, worldNormal);
            
            float normalizedDist = max(distSqr * rcpRangeSqr, 0.00001);
            attenuation *= texture(u_AttenuationLUT, normalizedDist.rr).r;
            attenuation *= shadowAttenuation;
            
            if (normalizedDist > 1.0 || dot(cookie, cookie) <= 0.0)
                discard;
            
            vec3 color = light.color.rgb * cookie;
        #endif // POINT || SPOT
        
        vec3 L = normalize(lightVec);
        
        vec3 Lo = PBR_DIRECT(albedo, metallic, roughness, viewDir, worldNormal, L, color, attenuation);
        
        // TODO: Fix shadows for spot lights
        // TODO: Only march through the cone of the spotlight
        // #if defined(SPOT)
        //     if (normalizedDist > 1.0 || dot(cookie, cookie) <= 0.0)
        //         Lo = vec3(0.0);
        // #endif
        
        return Lo;
    #elif defined(DIRECTIONAL) // Ambient lighting + directional
        vec3 Lo = vec3(0.0);
        
        // Volumetric parameters
        float stepSize = length(u_CameraPos - worldPos) / (NUM_VOLUMETRIC_STEPS + 1);
        float randOffset = rand3dTo1d(worldPos, vec3(12.989, 78.233, 37.719)) - 0.5;
        vec3 startPos = u_CameraPos + viewDir * stepSize * randOffset;
        
        for (int i = 0; i < lightAmount; i++) {
            // Calculate per-light radiance
            Light light = lights[i];
            vec3 L = normalize(-light.direction);
            float attenuation = PBR_SHADOW(light, worldPos, worldNormal);
            
            Lo += PBR_DIRECT(albedo, metallic, roughness, viewDir, worldNormal, L, light.color, attenuation);
            
            // Volumetric scattering
            Lo += VolumetricScattering(light, startPos, viewDir, worldNormal, L, stepSize, NUM_VOLUMETRIC_STEPS);
        }
        
        vec3 ambient = PBR_INDIRECT(albedo, metallic, roughness, ao, viewDir, worldNormal);
        
        return ambient + Lo;
    #else
        return vec3(0.0);
    #endif
}

#endif // PBR_INCLUDED