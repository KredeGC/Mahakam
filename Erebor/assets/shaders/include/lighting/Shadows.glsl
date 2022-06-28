#ifndef SHADOWS_INCLUDED
#define SHADOWS_INCLUDED

#if defined(DIRECTIONAL) || defined(SPOT)
    #define SHADOWS_ENABLED
#endif

#include "assets/shaders/include/lighting/LightStruct.glsl"
#include "assets/shaders/include/Random.glsl"

#if defined(SHADOWS_ENABLED)
    layout(binding = 8, location = 8) uniform sampler2D u_ShadowMap;
    
    #define FILTER_SIZE 1
    #define FILTER_SQUARE 9
    
    float SampleShadowDirect(vec2 projCoords, vec2 texSize, vec2 texelSize) {
        #if defined(BILINEAR_SHADOWS)
            vec2 f = fract(projCoords * texSize);
            projCoords += (0.5 - f) * texelSize;
            
            float tl = texture(u_ShadowMap, projCoords).r;
            float tr = texture(u_ShadowMap, projCoords + vec2(texelSize.x, 0.0)).r;
            float bl = texture(u_ShadowMap, projCoords + vec2(0.0, texelSize.y)).r;
            float br = texture(u_ShadowMap, projCoords + vec2(texelSize.x, texelSize.y)).r;
            
            float tA = mix(tl, tr, f.x);
            float tB = mix(bl, br, f.x);
            return mix(tA, tB, f.y);
        #else
            //return depth > texture(u_ShadowMap, projCoords).r ? 0.0 : 1.0;
            return texture(u_ShadowMap, projCoords).r;
        #endif
    }
    
    float SampleShadowMap(vec2 projCoords, float depth, vec2 texSize, vec2 texelSize) {
        #if defined(BILINEAR_SHADOWS)
            vec2 f = fract(projCoords * texSize);
            projCoords += (0.5 - f) * texelSize;
            
            float tl = depth > texture(u_ShadowMap, projCoords).r ? 0.0 : 1.0;
            float tr = depth > texture(u_ShadowMap, projCoords + vec2(texelSize.x, 0.0)).r ? 0.0 : 1.0;
            float bl = depth > texture(u_ShadowMap, projCoords + vec2(0.0, texelSize.y)).r ? 0.0 : 1.0;
            float br = depth > texture(u_ShadowMap, projCoords + vec2(texelSize.x, texelSize.y)).r ? 0.0 : 1.0;
            
            float tA = mix(tl, tr, f.x);
            float tB = mix(bl, br, f.x);
            return mix(tA, tB, f.y);
        #else
            return depth > texture(u_ShadowMap, projCoords).r ? 0.0 : 1.0;
        #endif
    }
    
    float EstimatePenumbra(float lightSize, float depth, vec2 projCoords, vec2 texSize, vec2 texelSize) {
        float avgDepth = 0.0;
        
        for (int x = -FILTER_SIZE; x <= FILTER_SIZE; x++) {
            for (int y = -FILTER_SIZE; y <= FILTER_SIZE; y++) {
                avgDepth += SampleShadowDirect(projCoords.xy + lightSize * vec2(x, y) * texelSize, texSize, texelSize).r;
            }
        }
        
        avgDepth /= FILTER_SQUARE;
        
        return max((depth - avgDepth) * lightSize / avgDepth, 0.0);
    }

    float SamplePCFShadow(vec2 projCoords, float depth, vec3 worldPos) {
        vec2 texSize = textureSize(u_ShadowMap, 0);
        vec2 texelSize = 1.0 / texSize;
        
        #if defined(PCSS_SHADOWS)
            const float lightSize = 4.0;
            
            float penumbra = EstimatePenumbra(lightSize, depth, projCoords, texSize, texelSize);
            
            float shadow = 0.0;
            for (int x = -FILTER_SIZE; x <= FILTER_SIZE; x++) {
                for (int y = -FILTER_SIZE; y <= FILTER_SIZE; y++) {
                    vec2 offset = vec2(x, y) * texelSize * penumbra;
                    vec2 randOffset = (rand3dTo2d(worldPos) * 2.0 - 1.0) * texelSize * penumbra * 0.1;
                    shadow += SampleShadowMap(projCoords.xy + offset + randOffset, depth, texSize, texelSize);
                }
            }
            
            return shadow / FILTER_SQUARE;
        #elif defined(PCF_SHADOWS)
            float shadow = 0.0;
            for (int x = -FILTER_SIZE; x <= FILTER_SIZE; x++) {
                for (int y = -FILTER_SIZE; y <= FILTER_SIZE; y++) {
                    vec2 offset = vec2(x, y) * texelSize;
                    vec2 randOffset = (rand3dTo2d(worldPos) * 2.0 - 1.0) * texelSize * 0.15;
                    shadow += SampleShadowMap(projCoords.xy + offset + randOffset, depth, texSize, texelSize);
                }
            }
            
            return shadow / FILTER_SQUARE;
        #else
            return SampleShadowMap(projCoords.xy, depth, texSize, texelSize);
        #endif
    }

    float CalculateShadowAttenuation(Light light, vec3 worldPos, vec3 normal) {
        if (light.offset.z == 0.0)
            return 1.0;
        
        #if defined(DIRECTIONAL)
            vec3 lightDir = normalize(-light.direction);
            // float bias = max(light.offset.w * (1.0 - dot(normal, lightDir)), light.offset.w * 0.1);
            
            float bias = light.offset.w;
            
            float texelSize = 0.5 * light.offset.z;
            //vec3 normalBias = normal * texelSize * 1.4142136;
            vec3 normalBias = vec3(0.0);
        #elif defined(SPOT)
            vec3 lightDir = normalize(light.objectToWorld[3].xyz - worldPos);
            
            // vec3 spotDir = mat3(light.objectToWorld) * vec3(0.0, 0.0, 1.0);
            // float distanceToLightPlane = dot(lightDir, spotDir);
            
            float bias = light.offset.w;
            
            vec3 normalBias = vec3(0.0);
        #endif
        
        // Transform worldPos into lightspace
        vec4 lightSpacePos = light.worldToLight * vec4(worldPos + normalBias, 1.0);
        lightSpacePos.xy /= lightSpacePos.w;
        
        // Offset xy into the depth texture atlas
        vec2 projCoords = lightSpacePos.xy * 0.5 + 0.5;
        float textureSize = textureSize(u_ShadowMap, 0).x;
        projCoords = projCoords * light.offset.z + light.offset.xy;
        
        // Calculate the comparative depth from lightSpacePos
        // lightSpacePos.z = min(lightSpacePos.z, lightSpacePos.w);
        float depth = (lightSpacePos.z - bias) / lightSpacePos.w;
        
        #if defined(DIRECTIONAL)
            depth = depth * 0.5 + 0.5;
        #endif
        
        // Sample shadow map and compare
        float shadow = SamplePCFShadow(projCoords, depth, worldPos);
        if (depth > 1.0 || abs(lightSpacePos.x) > 1.0 || abs(lightSpacePos.y) > 1.0)
            shadow = 1.0;
        
        return shadow;
    }
#endif

#endif // SHADOWS_INCLUDED