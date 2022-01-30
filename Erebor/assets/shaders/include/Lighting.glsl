#ifndef LIGHTING_INCLUDED
#define LIGHTING_INCLUDED

#include "assets/shaders/include/LightStruct.glsl"

const float PI = 3.14159265359;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.04);

layout(binding = 0) uniform samplerCube u_IrradianceMap;
layout(binding = 1) uniform samplerCube u_SpecularMap;
layout(binding = 2) uniform sampler2D u_BRDFLUT;
layout(binding = 3) uniform sampler2D u_AttenuationLUT;
layout(binding = 8) uniform sampler2D u_LightCookie;

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
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 depthToWorldSpace(vec2 uv, float depth) {
    vec4 clipSpaceLocation;
    clipSpaceLocation.xy = uv * 2.0 - 1.0;
    clipSpaceLocation.z = depth * 2.0 - 1.0;
    clipSpaceLocation.w = 1.0;
    vec4 homogenousLocation = MATRIX_IVP * clipSpaceLocation;
    return homogenousLocation.xyz / homogenousLocation.w;
}


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
            cookie = pow(cookie, vec3(2.2)); // sRGB correction?
            float attenuation = uvCookie.w > 0.0 ? 1.0 : 0.0;
            
            float normalizedDist = max(distSqr * rcpRangeSqr, 0.00001);
            attenuation *= texture(u_AttenuationLUT, normalizedDist.rr).r;
            
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
            vec3 radiance = light.color;

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
            vec3 diffuse = irradiance * albedo;
            
            // Indirect specular lighting
            const float MAX_REFLECTION_LOD = 4.0;
            vec3 prefilteredColor = textureLod(u_SpecularMap, R, roughness * MAX_REFLECTION_LOD).rgb;  
            vec2 brdf = texture(u_BRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
            vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
            
            ambient = (kD * diffuse + specular) * ao;
        }
        
        return ambient + Lo;
    #else
        return vec3(0.0);
    #endif
}

#endif // LIGHTING_INCLUDED