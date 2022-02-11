#type vertex
#version 450 core
#include "assets/shaders/include/Matrix.glsl"

#if defined(GEOMETRY)
    struct v2f {
        vec3 v_WorldPos;
        vec3 v_WorldNormal;
        vec3 v_WorldTangent;
        vec3 v_WorldBinormal;
        vec2 v_UV;
    };

    layout(location = 0) out v2f o;
#elif defined(SHADOW)
    layout (std140, binding = 1) uniform LightMatrices {
        mat4 u_WorldToLight;
    };
#endif

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;
layout(location = 3) in vec3 i_Tangent;
layout(location = 5) in ivec4 i_BoneIDs;
layout(location = 6) in vec4 i_BoneWeights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout(location = 6) uniform mat4 finalBonesMatrices[MAX_BONES];

void main() {
    // vec4 boneWeights = i_BoneWeights;
    // float size = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;
    // boneWeights = boneWeights / size;
    
    mat4 boneTransform = finalBonesMatrices[i_BoneIDs[0]] * i_BoneWeights[0];
    boneTransform += finalBonesMatrices[i_BoneIDs[1]] * i_BoneWeights[1];
    boneTransform += finalBonesMatrices[i_BoneIDs[2]] * i_BoneWeights[2];
    boneTransform += finalBonesMatrices[i_BoneIDs[3]] * i_BoneWeights[3];
    
    //mat3 normalMatrix = transpose(inverse(mat3(boneTransform)));
    vec4 pos = boneTransform * vec4(i_Pos, 1.0);
    
    #if defined(GEOMETRY)
        vec3 normal = mat3(boneTransform) * i_Normal;
        vec3 tangent = mat3(boneTransform) * i_Tangent;
        
        gl_Position = MATRIX_MVP * pos; //vec4(i_Pos, 1.0)
        
        o.v_WorldPos = (MATRIX_M * pos).xyz;
        //o.v_WorldNormal = (MATRIX_M * vec4(normal, 0.0)).xyz;
        o.v_WorldNormal = (normal * inverse(mat3(MATRIX_M))).xyz; // Correct for non-uniform scaled objects
        o.v_WorldTangent = (MATRIX_M * vec4(tangent, 0.0)).xyz;
        o.v_WorldBinormal = cross(o.v_WorldNormal, o.v_WorldTangent);
        o.v_UV = i_UV;
    #elif defined(SHADOW)
        gl_Position = u_WorldToLight * MATRIX_M * pos;
    #endif
}



#type fragment
#version 450 core
#include "assets/shaders/include/Matrix.glsl"

#if defined(GEOMETRY)
    struct v2f {
        vec3 v_WorldPos;
        vec3 v_WorldNormal;
        vec3 v_WorldTangent;
        vec3 v_WorldBinormal;
        vec2 v_UV;
    };

    layout(location = 0) in v2f i;
#endif

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Specular;
layout(location = 2) out vec4 o_Pos;
layout(location = 3) out vec4 o_Normal;

layout(binding = 0, location = 0) uniform sampler2D u_Albedo;
layout(binding = 1, location = 1) uniform sampler2D u_Bump;
layout(binding = 2, location = 2) uniform sampler2D u_Metallic;
layout(binding = 3, location = 3) uniform sampler2D u_Roughness;

vec3 unpackNormal(vec2 xy) {
    xy *= 2.0;
    xy -= 1.0;
    
    vec3 n;
    n.x = xy.x;
    n.y = xy.y;
    n.z = 1.0 - clamp(dot(n.xy, n.xy), 0.0, 1.0);
    return normalize(n);
}

void main() {
#if defined(GEOMETRY)
    // Surface values
    vec3 albedo = texture(u_Albedo, i.v_UV).rgb;
    vec3 bump = unpackNormal(texture(u_Bump, i.v_UV).xy);
    float metallic = texture(u_Metallic, i.v_UV).r;
    float roughness = texture(u_Roughness, i.v_UV).r;
    float ao = 1.0;
    
    //albedo = pow(albedo, vec3(2.2)); // sRGB correction
    
    //vec3 normal = normalize(i.v_WorldNormal);
    
    mat3 tbn = mat3(normalize(i.v_WorldTangent), normalize(i.v_WorldBinormal), normalize(i.v_WorldNormal));
    
    vec3 normal = normalize(tbn * bump);
    
    o_Albedo = vec4(albedo, ao);
    o_Specular = vec4(0.0, 0.0, metallic, roughness);
    o_Pos = vec4(i.v_WorldPos, 1.0);
    o_Normal = vec4(normal * 0.5 + 0.5, 0.0);
#endif
}