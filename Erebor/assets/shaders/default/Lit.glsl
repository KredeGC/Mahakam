#type vertex
#version 430 core
#include "assets/shaders/include/Matrix.glsl"

#ifdef SKIN
#include "assets/shaders/include/Skin.glsl"
#endif

#if defined(GEOMETRY)
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
#elif defined(SHADOW)
    layout (std140, binding = LIGHT_MATRICES_BINDING) uniform LightMatrices {
        mat4 u_WorldToLight;
    };
#endif // GEOMETRY || SHADOW

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;
layout(location = 3) in vec4 i_Tangent;
#ifdef SKIN
    layout(location = 5) in ivec4 i_BoneIDs;
    layout(location = 6) in vec4 i_BoneWeights;
#endif // SKIN

layout (std140) uniform Uniforms {
    vec4 u_UVTransform;
    vec3 u_Color;
    vec3 u_EmissionColor;
    float u_MetallicMul;
    float u_RoughnessMul;
};

void main() {
    #ifdef SKIN
        mat4 boneTransform = transformBoneVertex(i_BoneIDs, i_BoneWeights);

        vec3 pos = (boneTransform * vec4(i_Pos, 1.0)).xyz;
        vec3 normal = mat3(boneTransform) * i_Normal;
        vec3 tangent = mat3(boneTransform) * i_Tangent.xyz;
    #else
        vec3 pos = i_Pos;
        vec3 normal = i_Normal;
        vec3 tangent = i_Tangent.xyz;
    #endif // SKIN
    
    #if defined(GEOMETRY)
        gl_Position = MATRIX_MVP * vec4(pos, 1.0);

        o.v_WorldPos = transformWorldPos(MATRIX_M, pos);
        o.v_WorldNormal = transformWorldNormal(MATRIX_M, normal);
        #ifndef USE_TRIPLANAR
            o.v_WorldTangent = transformWorldTangent(MATRIX_M, tangent);
            o.v_WorldBinormal = transformWorldBinormal(o.v_WorldNormal, o.v_WorldTangent, i_Tangent.w);
            o.v_UV = transformTexCoordinates(i_UV, u_UVTransform);
        #endif
    #elif defined(SHADOW)
        gl_Position = u_WorldToLight * MATRIX_M * vec4(pos, 1.0);
    #endif // GEOMETRY || SHADOW
}



#type fragment
#version 430 core
#include "assets/shaders/include/Matrix.glsl"

#ifdef USE_BUMP
#include "assets/shaders/include/Normal.glsl"
#endif // USE_BUMP

#ifdef USE_STOCHASTIC
#include "assets/shaders/include/Stochastic.glsl"
#define SAMPLE_TEXTURE SampleStochastic
#else
#define SAMPLE_TEXTURE texture
#endif // USE_STOCHASTIC

#ifdef USE_TRIPLANAR
#include "assets/shaders/include/Triplanar.glsl"
#endif // USE_TRIPLANAR

#ifdef GEOMETRY
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
#endif // GEOMETRY

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Specular;
layout(location = 2) out vec4 o_Emission;
layout(location = 3) out vec4 o_Normal;

#ifdef USE_ALBEDO
    layout(binding = 0, location = 0) uniform sampler2D u_Albedo;
#endif // USE_ALBEDO
#ifdef USE_BUMP
    layout(binding = 1, location = 1) uniform sampler2D u_Bump;
#endif // USE_BUMP
#ifdef USE_METALLIC
    layout(binding = 2, location = 2) uniform sampler2D u_Metallic;
#endif // USE_METALLIC
#ifdef USE_ROUGHNESS
    layout(binding = 3, location = 3) uniform sampler2D u_Roughness;
#endif // USE_ROUGHNESS
#ifdef USE_OCCLUSSION
    layout(binding = 4, location = 4) uniform sampler2D u_Occlussion;
#endif // USE_OCCLUSSION
#ifdef USE_EMISSION
    layout(binding = 5, location = 5) uniform sampler2D u_Emission;
#endif // USE_EMISSION

layout (std140) uniform Uniforms {
    vec4 u_UVTransform;
    vec3 u_Color;
    vec3 u_EmissionColor;
    float u_MetallicMul;
    float u_RoughnessMul;
};

void main() {
#ifdef GEOMETRY
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
#endif
}