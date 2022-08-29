#type vertex
#version 430 core
#include "assets/shaders/include/Matrix.glsl"

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    #ifndef USE_TRIPLANAR
        vec3 v_WorldTangent;
        vec3 v_WorldBinormal;
        vec2 v_UV;
    #endif
};

layout(location = 0) out v2f o;

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;
layout(location = 3) in vec4 i_Tangent;

#ifndef USE_TRIPLANAR
    layout(location = 0) uniform vec4 u_UVTransform;
#endif

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
    
    o.v_WorldPos = (MATRIX_M * vec4(i_Pos, 1.0)).xyz;
    //o.v_WorldNormal = (MATRIX_M * vec4(i_Normal, 0.0)).xyz; // Correct for uniformly scaled objects
    o.v_WorldNormal = (vec4(i_Normal, 0.0) * inverse(MATRIX_M)).xyz; // Correct for non-uniform scaled objects
    #ifndef USE_TRIPLANAR
        o.v_WorldTangent = (MATRIX_M * vec4(i_Tangent.xyz, 0.0)).xyz;
        o.v_WorldBinormal = cross(o.v_WorldNormal, o.v_WorldTangent.xyz);
        o.v_UV = u_UVTransform.xy * i_UV + u_UVTransform.zw;
    #endif
}



#type fragment
#version 430 core
#include "assets/shaders/include/Matrix.glsl"

#ifdef USE_BUMP
#include "assets/shaders/include/Normal.glsl"
#endif

#ifdef USE_STOCHASTIC
#include "assets/shaders/include/Stochastic.glsl"
#define SAMPLE_TEXTURE SampleStochastic
#else
#define SAMPLE_TEXTURE texture
#endif

#ifdef USE_TRIPLANAR
#include "assets/shaders/include/Triplanar.glsl"
#endif

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    #ifndef USE_TRIPLANAR
        vec3 v_WorldTangent;
        vec3 v_WorldBinormal;
        vec2 v_UV;
    #endif
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Specular;
layout(location = 2) out vec4 o_Emission;
layout(location = 3) out vec4 o_Normal;

layout(location = 0) uniform vec4 u_UVTransform;
#ifdef USE_ALBEDO
    layout(binding = 1, location = 1) uniform sampler2D u_Albedo;
#endif
layout(location = 2) uniform vec3 u_Color;
#ifdef USE_BUMP
    layout(binding = 3, location = 3) uniform sampler2D u_Bump;
#endif
#ifdef USE_METALLIC
    layout(binding = 4, location = 4) uniform sampler2D u_Metallic;
#endif
layout(location = 5) uniform float u_MetallicMul;
#ifdef USE_ROUGHNESS
    layout(binding = 6, location = 6) uniform sampler2D u_Roughness;
#endif
layout(location = 7) uniform float u_RoughnessMul;
#ifdef USE_OCCLUSSION
    layout(binding = 8, location = 8) uniform sampler2D u_Occlussion;
#endif
#ifdef USE_EMISSION
    layout(binding = 9, location = 9) uniform sampler2D u_Emission;
#endif
layout(location = 10) uniform vec3 u_EmissionColor;

void main() {
    vec3 worldNormal = normalize(i.v_WorldNormal);
    
    // Triplanar values
    #ifdef USE_TRIPLANAR
        vec3 axisSign = CalculateAxisSign(worldNormal);
        vec2 uvX, uvY, uvZ;
        CreateTriplanarUVs(i.v_WorldPos, worldNormal, axisSign, u_UVTransform, uvX, uvY, uvZ);
        vec3 blend = TriplanarWeights(worldNormal);
    #endif // USE_TRIPLANAR
    
    // Albedo texture
    #ifdef USE_ALBEDO
        #ifdef USE_TRIPLANAR
            vec3 albedo = SampleTriplanar(u_Albedo, uvX, uvY, uvZ, blend).rgb;
        #else
            vec3 albedo = SAMPLE_TEXTURE(u_Albedo, i.v_UV).rgb;
        #endif // USE_TRIPLANAR
        
        //albedo = pow(albedo, vec3(2.2)); // sRGB correction
    #else
        vec3 albedo = vec3(1.0, 1.0, 1.0);
    #endif // USE_ALBEDO
    
    // Bumpmap texture
    #ifdef USE_BUMP
        #ifdef USE_TRIPLANAR
            vec3 normal = SampleTriplanarNormal(u_Bump, uvX, uvY, uvZ, worldNormal, axisSign, blend);
        #else
            vec3 bump = UnpackNormal(SAMPLE_TEXTURE(u_Bump, i.v_UV).xy);
            mat3 tbn = mat3(normalize(i.v_WorldTangent), normalize(i.v_WorldBinormal), normalize(i.v_WorldNormal));
            vec3 normal = normalize(tbn * bump);
        #endif // USE_TRIPLANAR
    #else
        vec3 normal = worldNormal;
    #endif // USE_BUMP
    
    // Metallic texture
    #ifdef USE_METALLIC
        #ifdef USE_TRIPLANAR
            float metallic = SampleTriplanar(u_Metallic, uvX, uvY, uvZ, blend).r;
        #else
            float metallic = SAMPLE_TEXTURE(u_Metallic, i.v_UV).r;
        #endif // USE_TRIPLANAR
    #else
        float metallic = 1.0;
    #endif // USE_METALLIC
    
    // Roughness texture
    #ifdef USE_ROUGHNESS
        #ifdef USE_TRIPLANAR
            float roughness = SampleTriplanar(u_Roughness, uvX, uvY, uvZ, blend).r;
        #else
            float roughness = SAMPLE_TEXTURE(u_Roughness, i.v_UV).r;
        #endif // USE_TRIPLANAR
    #else
        float roughness = 1.0;
    #endif // USE_ROUGHNESS
    
    // Occlussion texture
    #ifdef USE_OCCLUSSION
        #ifdef USE_TRIPLANAR
            float ao = SampleTriplanar(u_Occlussion, uvX, uvY, uvZ, blend).r;
        #else
            float ao = SAMPLE_TEXTURE(u_Occlussion, i.v_UV).r;
        #endif // USE_TRIPLANAR
    #else
        float ao = 1.0;
    #endif // USE_OCCLUSSION
    
    // Emission texture
    #ifdef USE_EMISSION
        #ifdef USE_TRIPLANAR
            vec3 emission = SampleTriplanar(u_Emission, uvX, uvY, uvZ, blend).rgb;
        #else
            vec3 emission = SAMPLE_TEXTURE(u_Emission, i.v_UV).rgb;
        #endif // USE_TRIPLANAR
    #else
        vec3 emission = vec3(1.0, 1.0, 1.0);
    #endif // USE_EMISSION
    
    // Combine with multipliers
    albedo *= u_Color;
    metallic *= u_MetallicMul;
    roughness *= u_RoughnessMul;
    emission *= u_EmissionColor;
    
    o_Albedo = vec4(albedo, ao);
    o_Specular = vec4(0.0, 0.0, metallic, roughness);
    o_Emission = vec4(emission, 0.0);
    o_Normal = vec4(normal * 0.5 + 0.5, 0.0);
}