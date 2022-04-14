#ifndef LIGHTING_INCLUDED
#define LIGHTING_INCLUDED

#include "assets/shaders/include/LightStruct.glsl"

const float PI = 3.14159265359;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.04);

layout(binding = 0, location = 0) uniform samplerCube u_IrradianceMap;
layout(binding = 1, location = 1) uniform samplerCube u_SpecularMap;
layout(binding = 2, location = 2) uniform sampler2D u_BRDFLUT;
layout(binding = 3, location = 3) uniform sampler2D u_AttenuationLUT;
layout(binding = 10, location = 10) uniform sampler2D u_LightCookie;
layout(binding = 11, location = 11) uniform sampler2D u_ShadowMap;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    vec3 inv = 1.0 - F0;
    float term = pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
    return F0 + inv * term;
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    vec3 inv = max(vec3(1.0 - roughness), F0) - F0;
    float term = pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
    return F0 + inv * term;
}

vec3 depthToWorldSpace(vec2 uv, float depth) {
    vec4 clipSpaceLocation;
    clipSpaceLocation.xy = uv * 2.0 - 1.0;
    clipSpaceLocation.z = depth * 2.0 - 1.0;
    clipSpaceLocation.w = 1.0;
    vec4 homogenousLocation = MATRIX_IVP * clipSpaceLocation;
    return homogenousLocation.xyz / homogenousLocation.w;
}

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


#if defined(DIRECTIONAL) || defined(SPOT)
    #define FILTER_SIZE 1
    #define FILTER_SQUARE 9
    
    float SampleShadowDirect(vec2 projCoords, vec2 texSize, vec2 texelSize) {
        #if defined(BILINEAR_SHADOWS)
            vec2 f = fract(projCoords * texSize);
            projCoords += (0.5 - f) * texelSize;
            
            float tl = texture2D(u_ShadowMap, projCoords).r;
            float tr = texture2D(u_ShadowMap, projCoords + vec2(texelSize.x, 0.0)).r;
            float bl = texture2D(u_ShadowMap, projCoords + vec2(0.0, texelSize.y)).r;
            float br = texture2D(u_ShadowMap, projCoords + vec2(texelSize.x, texelSize.y)).r;
            
            float tA = mix(tl, tr, f.x);
            float tB = mix(bl, br, f.x);
            return mix(tA, tB, f.y);
        #else
            return depth > texture2D(u_ShadowMap, projCoords).r ? 0.0 : 1.0;
        #endif
    }
    
    float SampleShadowMap(vec2 projCoords, float depth, vec2 texSize, vec2 texelSize) {
        #if defined(BILINEAR_SHADOWS)
            vec2 f = fract(projCoords * texSize);
            projCoords += (0.5 - f) * texelSize;
            
            float tl = depth > texture2D(u_ShadowMap, projCoords).r ? 0.0 : 1.0;
            float tr = depth > texture2D(u_ShadowMap, projCoords + vec2(texelSize.x, 0.0)).r ? 0.0 : 1.0;
            float bl = depth > texture2D(u_ShadowMap, projCoords + vec2(0.0, texelSize.y)).r ? 0.0 : 1.0;
            float br = depth > texture2D(u_ShadowMap, projCoords + vec2(texelSize.x, texelSize.y)).r ? 0.0 : 1.0;
            
            float tA = mix(tl, tr, f.x);
            float tB = mix(bl, br, f.x);
            return mix(tA, tB, f.y);
        #else
            return depth > texture2D(u_ShadowMap, projCoords).r ? 0.0 : 1.0;
        #endif
    }
    
    float rand3dTo1d(vec3 value, vec3 dotDir = vec3(12.9898, 78.233, 37.719)) {
        vec3 smallValue = cos(value);
        float random = dot(smallValue, dotDir);
        random = fract(sin(random) * 143758.5453);
        return random;
    }
    
    vec2 rand3dTo2d(vec3 value) {
        return vec2(
            rand3dTo1d(value, vec3(12.989, 78.233, 37.719)),
            rand3dTo1d(value, vec3(39.346, 11.135, 83.155))
        );
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
        #if defined(PCSS_SHADOWS)
            const float lightSize = 4.0;
            vec2 texSize = textureSize(u_ShadowMap, 0);
            vec2 texelSize = 1.0 / texSize;
            
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
            vec2 texSize = textureSize(u_ShadowMap, 0);
            vec2 texelSize = 1.0 / texSize;
            for (int x = -FILTER_SIZE; x <= FILTER_SIZE; x++) {
                for (int y = -FILTER_SIZE; y <= FILTER_SIZE; y++) {
                    vec2 offset = vec2(x, y) * texelSize;
                    vec2 randOffset = (rand3dTo2d(worldPos) * 2.0 - 1.0) * texelSize * 0.15;
                    shadow += SampleShadowMap(projCoords.xy + offset + randOffset, depth, texSize, texelSize);
                }
            }
            
            return shadow / FILTER_SQUARE;
        #else
            return SampleShadowMap(projCoords.xy, depth);
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
            vec3 normalBias = normal * texelSize * 1.4142136;
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


vec3 BRDF(vec3 albedo, float metallic, float roughness, float ao, vec3 viewDir, vec3 worldPos, vec3 worldNormal) {
    vec3 N = worldNormal;
    vec3 V = viewDir;
    vec3 R = reflect(-V, N);
    
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = mix(Fdielectric, albedo, metallic);
    
    #if defined(POINT) || defined(SPOT) // Direct lighting
        Light light = lights[v_InstanceID];
        // calculate per-light radiance
        #if defined(POINT)
            vec3 lightVec = light.position.xyz - worldPos;
            vec3 L = normalize(lightVec);
            vec3 H = normalize(V + L);
            
            float distSqr = dot(lightVec, lightVec);
            float rcpRangeSqr = light.color.w;
            
            float normalizedDist = max(distSqr * rcpRangeSqr, 0.00001);
            float attenuation = texture(u_AttenuationLUT, normalizedDist.rr).r;
            
            if (normalizedDist > 1.0) // Find something that saves more performance?
                discard;
            
            vec3 radiance = light.color.rgb * attenuation;
        #elif defined(SPOT)
            vec3 lightVec = light.objectToWorld[3].xyz - worldPos;
            vec3 L = normalize(lightVec);
            vec3 H = normalize(V + L);
            
            float distSqr = dot(lightVec, lightVec);
            float rcpRangeSqr = light.color.w;
            
            vec4 uvCookie = light.worldToLight * vec4(worldPos, 1.0);
            // negative bias because http://aras-p.info/blog/2010/01/07/screenspace-vs-mip-mapping/
            vec3 cookie = texture(u_LightCookie, uvCookie.xy / uvCookie.w * 0.5 + vec2(0.5, 0.5), -8.0).rgb;
            float attenuation = uvCookie.w > 0.0 ? 1.0 : 0.0;
            
            float shadowAttenuation = CalculateShadowAttenuation(light, worldPos, N);
            
            float normalizedDist = max(distSqr * rcpRangeSqr, 0.00001);
            attenuation *= texture(u_AttenuationLUT, normalizedDist.rr).r;
            attenuation *= shadowAttenuation;
            
            if (normalizedDist > 1.0 || dot(cookie, cookie) <= 0.0) // Find something that saves more performance?
                discard;
            
            vec3 radiance = light.color.rgb * cookie * attenuation;
        #endif

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
        
        vec3 numerator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        
        return Lo;
    #elif defined(DIRECTIONAL) // Ambient lighting + directional
        vec3 Lo = vec3(0.0);
        for (int i = 0; i < lightAmount; i++) {
            Light light = lights[i];
            // calculate per-light radiance
            vec3 L = normalize(-light.direction);
            vec3 H = normalize(V + L);
            float shadowAttenuation = CalculateShadowAttenuation(light, worldPos, N);
            vec3 radiance = light.color * shadowAttenuation;

            // Cook-Torrance BRDF
            float NDF = DistributionGGX(N, H, roughness);
            float G   = GeometrySmith(N, V, L, roughness);
            vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
            
            vec3 numerator    = NDF * G * F; 
            float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
            vec3 specular = numerator / denominator;
            
            // kS is equal to Fresnel
            vec3 kS = F;
            // for energy conservation, the diffuse and specular light can't
            // be above 1.0 (unless the surface emits light); to preserve this
            // relationship the diffuse component (kD) should equal 1.0 - kS.
            vec3 kD = 1.0 - kS;
            // multiply kD by the inverse metalness such that only non-metals
            // have diffuse lighting, or a linear blend if partly metal (pure metals
            // have no diffuse light).
            kD *= 1.0 - metallic;

            // scale light by NdotL
            float NdotL = max(dot(N, L), 0.0);

            // add to outgoing radiance Lo
            Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        }
        
        vec3 ambient;
        {
            // Indirect diffuse lighting
            vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

            vec3 kS = F;
            vec3 kD = 1.0 - kS;
            kD *= 1.0 - metallic;
            vec3 irradiance = texture(u_IrradianceMap, N).rgb;
            vec3 diffuse = irradiance * albedo * ao;
            
            // Indirect specular lighting
            const float MAX_REFLECTION_LOD = 4.0;
            vec3 prefilteredColor = textureLod(u_SpecularMap, R, roughness * MAX_REFLECTION_LOD).rgb;  
            vec2 brdf = texture(u_BRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
            vec3 specular = prefilteredColor * ao * (F * brdf.x + brdf.y);
            
            ambient = (kD * diffuse + specular) * ao;
        }
        
        return ambient + Lo;
    #else
        return vec3(0.0);
    #endif
}

#endif // LIGHTING_INCLUDED