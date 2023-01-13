#ifndef SHADOWS_INCLUDED
#define SHADOWS_INCLUDED

#if defined(DIRECTIONAL) || defined(SPOT)
    #define SHADOWS_ENABLED
#endif

#include "internal/shaders/include/lighting/LightStruct.glsl"
#include "internal/shaders/include/Random.glsl"

#if defined(SHADOWS_ENABLED)
    layout(binding = 8, location = 8) uniform sampler2D u_ShadowMap;
    
    #define FILTER_SIZE 1
    #define FILTER_SQUARE 9
    
    float SampleShadowMapBilinear(vec2 projCoords, float depth, vec2 texSize, vec2 texelSize) {
        vec2 f = fract(projCoords * texSize);
        projCoords += (0.5 - f) * texelSize;
        
        float tl = depth > texture(u_ShadowMap, projCoords).r ? 0.0 : 1.0;
        float tr = depth > texture(u_ShadowMap, projCoords + vec2(texelSize.x, 0.0)).r ? 0.0 : 1.0;
        float bl = depth > texture(u_ShadowMap, projCoords + vec2(0.0, texelSize.y)).r ? 0.0 : 1.0;
        float br = depth > texture(u_ShadowMap, projCoords + vec2(texelSize.x, texelSize.y)).r ? 0.0 : 1.0;
        
        float tA = mix(tl, tr, f.x);
        float tB = mix(bl, br, f.x);
        return mix(tA, tB, f.y);
    }
    
    float SampleShadowMapDirect(vec2 projCoords, float depth, vec2 texSize, vec2 texelSize) {
        #if defined(BILINEAR_SHADOWS)
            return SampleShadowMapBilinear(projCoords, depth, texSize, texelSize);
        #else
            return depth > texture(u_ShadowMap, projCoords).r ? 0.0 : 1.0;
        #endif
    }

    float SamplePCFShadow(vec2 projCoords, float depth, vec3 worldPos, vec2 texSize, vec2 texelSize) {
        #if defined(PCF_SHADOWS)
            float shadow = 0.0;
            for (int x = -FILTER_SIZE; x <= FILTER_SIZE; x++) {
                for (int y = -FILTER_SIZE; y <= FILTER_SIZE; y++) {
                    vec2 offset = vec2(x, y) * texelSize;
                    vec2 randOffset = (rand3dTo2d(worldPos) * 2.0 - 1.0) * texelSize * 0.15;
                    shadow += SampleShadowMapDirect(projCoords.xy + offset + randOffset, depth, texSize, texelSize);
                }
            }
            
            return shadow / FILTER_SQUARE;
        #else
            return SampleShadowMapDirect(projCoords.xy, depth, texSize, texelSize);
        #endif
    }
    
    vec3 CalculateShadowDirection(Light light, vec3 worldPos) {
        #if defined(DIRECTIONAL)
            return normalize(-light.direction);
        #elif defined(SPOT)
            return normalize(light.objectToWorld[3].xyz - worldPos);
        #endif
    }
    
    vec3 CalculateShadowNormalBias(Light light, vec3 worldPos, vec3 normal, float texSize) {
        #if defined(DIRECTIONAL)
            vec3 normalBias = normal * light.offset.w * light.offset.z;
        #elif defined(SPOT)
            vec3 distSurface = light.objectToWorld[3].xyz - worldPos;
            vec3 spotDir = normalize(mat3(light.objectToWorld) * vec3(0.0, 0.0, 1.0));
            float distanceToLightPlane = dot(distSurface, spotDir);
            
            vec3 normalBias = normal * light.offset.w * light.offset.z * distanceToLightPlane;
        #endif
        
        return normalBias;
    }
    
    float CalculateShadowAttenuationRaw(Light light, vec3 worldPos, vec3 normal) {
        if (light.offset.z == 0.0)
            return 1.0;
        
        vec2 texSize = textureSize(u_ShadowMap, 0);
        vec2 texelSize = 1.0 / texSize;
        
        // Calculate various properties of the light
        vec3 lightDir = CalculateShadowDirection(light, worldPos);
        float bias = 0.0;
        vec3 normalBias = CalculateShadowNormalBias(light, worldPos, normal, texSize.x);
        
        // Transform worldPos into lightspace
        vec4 lightSpacePos = light.worldToLight * vec4(worldPos + normalBias, 1.0);
        lightSpacePos.xy /= lightSpacePos.w;
        
        // Offset xy into the depth texture atlas
        vec2 projCoords = lightSpacePos.xy * 0.5 + 0.5;
        projCoords = projCoords * light.offset.z + light.offset.xy;
        
        // Calculate the comparative depth from lightSpacePos
        float depth = (lightSpacePos.z - bias) / lightSpacePos.w;
        
        // Transform to 0-1 range
        depth = depth * 0.5 + 0.5;
        
        // Sample shadow map and compare
        float shadow = depth > texture(u_ShadowMap, projCoords).r ? 0.0 : 1.0;
        if (depth > 1.0 || abs(lightSpacePos.x) > 1.0 || abs(lightSpacePos.y) > 1.0)
            shadow = 1.0;
        
        return shadow;
    }

    float CalculateShadowAttenuation(Light light, vec3 worldPos, vec3 normal) {
        if (light.offset.z == 0.0)
            return 1.0;
        
        vec2 texSize = textureSize(u_ShadowMap, 0);
        vec2 texelSize = 1.0 / texSize;
        
        // Calculate various properties of the light
        vec3 lightDir = CalculateShadowDirection(light, worldPos);
        float bias = 0.0;
        vec3 normalBias = CalculateShadowNormalBias(light, worldPos, normal, texSize.x);
        
        // Transform worldPos into lightspace
        vec4 lightSpacePos = light.worldToLight * vec4(worldPos + normalBias, 1.0);
        lightSpacePos.xy /= lightSpacePos.w;
        
        // Offset xy into the depth texture atlas
        vec2 projCoords = lightSpacePos.xy * 0.5 + 0.5;
        projCoords = projCoords * light.offset.z + light.offset.xy;
        
        // Calculate the comparative depth from lightSpacePos
        float depth = (lightSpacePos.z - bias) / lightSpacePos.w;
        
        // Transform to 0-1 range
        depth = depth * 0.5 + 0.5;
        
        // Sample shadow map and compare
        float shadow = SamplePCFShadow(projCoords, depth, worldPos, texSize, texelSize);
        if (depth > 1.0 || abs(lightSpacePos.x) > 1.0 || abs(lightSpacePos.y) > 1.0)
            shadow = 1.0;
        
        return shadow;
    }
#endif

#endif // SHADOWS_INCLUDED