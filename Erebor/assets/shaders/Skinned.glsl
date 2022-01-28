#type vertex
#version 450 core
#include "assets/shaders/include/Matrix.glsl"

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec2 v_UV;
};

layout(location = 0) out v2f o;

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;
layout(location = 3) in ivec4 i_BoneIDs;
layout(location = 4) in vec4 i_BoneWeights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 finalBonesMatrices[MAX_BONES];

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
    vec3 normal = mat3(boneTransform) * i_Normal;
    
    gl_Position = MATRIX_MVP * pos; //vec4(i_Pos, 1.0)
    
    o.v_WorldPos = (MATRIX_M * pos).xyz;
    //o.v_WorldNormal = (MATRIX_M * vec4(normal, 0.0)).xyz;
    o.v_WorldNormal = (normal * inverse(mat3(MATRIX_M))).xyz; // Correct for non-uniform scaled objects
    o.v_UV = i_UV;
}



#type fragment
#version 450 core
#include "assets/shaders/include/Matrix.glsl"

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec2 v_UV;
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Specular;
layout(location = 2) out vec4 o_Pos;
layout(location = 3) out vec4 o_Normal;

layout(binding = 3) uniform sampler2D u_Albedo;
layout(binding = 4) uniform sampler2D u_Metallic;
layout(binding = 5) uniform sampler2D u_Roughness;
uniform float u_AO; // ??

void main() {
    // Surface values
    vec3 albedo = vec3(1.0, 1.0, 1.0); //texture(u_Albedo, i.v_UV).rgb;
    float metallic = 0.0; //texture(u_Metallic, i.v_UV).r;
    float roughness = 0.5; //texture(u_Roughness, i.v_UV).r;
    float ao = 1.0;
    
    vec3 normal = normalize(i.v_WorldNormal);
    
    o_Albedo = vec4(albedo, ao);
    o_Specular = vec4(0.5, 0.5, metallic, roughness);
    o_Pos = vec4(i.v_WorldPos, 1.0);
    o_Normal = vec4(normal * 0.5 + 0.5, 0.0);
}