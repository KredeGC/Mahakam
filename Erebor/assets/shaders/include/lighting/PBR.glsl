#ifndef PBR_INCLUDED
#define PBR_INCLUDED

#include "assets/shaders/include/lighting/LightStruct.glsl"

layout(binding = 9, location = 9) uniform sampler2D u_AttenuationLUT;
layout(binding = 10, location = 10) uniform sampler2D u_LightCookie;

// vec3 depthToViewSpace(vec2 uv, float depth) {
//     vec4 clipSpaceLocation;
//     clipSpaceLocation.xy = uv * 2.0 - 1.0;
//     clipSpaceLocation.z = depth * 2.0 - 1.0;
//     clipSpaceLocation.w = 1.0;
//     vec4 homogenousLocation = MATRIX_IP * clipSpaceLocation;
//     return homogenousLocation.xyz / homogenousLocation.w;
// }

// #if defined(DIRECTIONAL)
//     vec2 RaymarchReflections(vec2 startUV, float viewspaceDepth, vec3 viewDir, vec3 worldNormal) {
//         const vec3 reflDir = normalize(reflect(viewDir, worldNormal));
        
//         vec4 reflClipDir = MATRIX_VP * reflDir;
//         reflClipDir.xyz / reflClipDir.w;
//         vec2 endUV = reflClipDir.xy * 0.5 + 0.5;
        
//         const vec2 textureSize = textureSize(u_GBuffer0, 0);
//         const vec2 texelSize = 1.0 / textureSize;
        
//         startUV *= textureSize;
//         endUV *= textureSize;
        
//         const float deltaX = endUV.x - startUV.x;
//         const float deltaY = endUV.y - startUV.y;
        
//         const float scaleX = sqrt(1.0 + pow(deltaY / deltaX, 2.0));
//         const float scaleY = sqrt(1.0 + pow(deltaX / deltaY, 2.0));
        
//         const vec2 stepX = deltaX > 0.0 ? vec2(1.0, 0.0) : vec2(-1.0, 0.0);
//         const vec2 stepY = deltaY > 0.0 ? vec2(0.0, 1.0) : vec2(0.0, -1.0);
        
//         vec2 deltaUV = startUV;
//         vec3 pos = depthToViewSpace(startUV * texelSize, viewspaceDepth);
        
//         for (int i = 0; i < 50; i++) {
//             float useX = xPos < yPos ? 1.0 : 0.0;
            
//             deltaUV += mix(stepX * scaleX, stepY * scaleY, useX);
            
//             pos += reflClipDir;
            
//             vec2 uv = pos.xy;
            
//             float testDepth = texture(u_Depth, uv).r;
//             //vec3 worldPos = depthToViewSpace(uv, depth);
            
//             if (depth < 0.0) {
                
//             }
//         }
//     }
// #endif


vec3 BRDF(vec3 albedo, float metallic, float roughness, float ao, vec3 viewDir, vec3 worldPos, vec3 worldNormal) {
    #if defined(POINT) || defined(SPOT) // Direct lighting
        Light light = lights[v_InstanceID];
        // calculate per-light radiance
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
            // negative bias because http://aras-p.info/blog/2010/01/07/screenspace-vs-mip-mapping/
            vec3 cookie = texture(u_LightCookie, uvCookie.xy / uvCookie.w * 0.5 + vec2(0.5, 0.5), -8.0).rgb;
            float attenuation = uvCookie.w > 0.0 ? 1.0 : 0.0;
            
            float shadowAttenuation = PBR_SHADOW(light, worldPos, worldNormal);
            
            float normalizedDist = max(distSqr * rcpRangeSqr, 0.00001);
            attenuation *= texture(u_AttenuationLUT, normalizedDist.rr).r;
            attenuation *= shadowAttenuation;
            
            if (normalizedDist > 1.0 || dot(cookie, cookie) <= 0.0) // Find something that saves more performance?
                discard;
            
            vec3 color = light.color.rgb * cookie;
        #endif
        
        vec3 L = normalize(lightVec);
        
        return PBR_DIRECT(albedo, metallic, roughness, viewDir, worldNormal, L, color, attenuation);
    #elif defined(DIRECTIONAL) // Ambient lighting + directional
        vec3 Lo = vec3(0.0);
        for (int i = 0; i < lightAmount; i++) {
            Light light = lights[i];
            // calculate per-light radiance
            vec3 L = normalize(-light.direction);
            float attenuation = PBR_SHADOW(light, worldPos, worldNormal);
            
            Lo += PBR_DIRECT(albedo, metallic, roughness, viewDir, worldNormal, L, light.color, attenuation);
        }
        
        vec3 ambient = PBR_INDIRECT(albedo, metallic, roughness, ao, viewDir, worldNormal);
        
        return ambient + Lo;
    #else
        return vec3(0.0);
    #endif
}

#endif // PBR_INCLUDED