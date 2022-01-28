#if defined(POINT)
    struct Light {
        vec4 position; // xyz - pos, w - light range/distance
        vec3 color;
        //mat4 worldToLight;
    };
#elif defined(SPOT)
    struct Light {
        vec4 position; // xyz - pos, w - light range/distance
        vec3 direction;
        vec3 color;
        float cutoff;
        //mat4 worldToLight;
    };
#else // DIRECTIONAL
    struct Light {
        vec3 direction;
        vec3 color;
        //mat4 worldToLight;
    };
#endif


// layout (std140, binding = 1) uniform Lights {
//     Light light;
// };

layout (std430, binding = 1) buffer Lights {
    int lightAmount;
    Light[] lights;
};

const float PI = 3.14159265359;
const float Epsilon = 0.00001;
const vec3 Fdielectric = vec3(0.04);

layout(binding = 0) uniform samplerCube u_IrradianceMap;
layout(binding = 1) uniform samplerCube u_SpecularMap;
layout(binding = 2) uniform sampler2D u_BRDFLUT;
layout(binding = 3) uniform sampler2D u_AttenuationLUT;

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

// Calculate attenuated light falloff
float calculateLightFalloff(float dist, float outerRadius) {
	float normalizedDist = dist / outerRadius;
	return clamp(1.0 / (1.0 + 25.0 * normalizedDist * normalizedDist) * clamp((1.0 - normalizedDist) * 5.0, 0.0, 1.0), 0.0, 1.0);
}


vec3 BRDF(vec3 albedo, float metallic, float roughness, float ao, vec3 viewDir, vec3 worldPos, vec3 worldNormal) {
    vec3 N = worldNormal;
    vec3 V = viewDir;
    vec3 R = reflect(-V, N);
    
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = mix(Fdielectric, albedo, metallic);

    // Direct lighting
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < lightAmount; i++) {
        Light light = lights[i];
        // calculate per-light radiance
        #if defined(POINT)
            vec3 lightVec = light.position.xyz - worldPos;
            vec3 L = normalize(lightVec);
            vec3 H = normalize(V + L);
            
            float distSqr = dot(lightVec, lightVec);
            
            float normalizedDist = distSqr * light.position.w;
            float attenuation = texture(u_AttenuationLUT, (distSqr * light.position.w).rr).r;
            
            if (normalizedDist > 1.0) // Find something that saves more performance?
                continue;
            
            vec3 radiance = light.color * attenuation;
        #else
            vec3 L = normalize(-light.direction);
            vec3 H = normalize(V + L);
            vec3 radiance = light.color;
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
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }
    
    
    // Ambient lighting
    #if defined(DIRECTIONAL)
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
        
        vec3 ambient = (kD * diffuse + specular) * ao;
        
        Lo += ambient;
    }
    #endif
    

    // Combine direct and indirect lighting
	return Lo;
}