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
    vec4 pos = vec4(0.0);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (i_BoneIDs[i] == -1) continue;
        
        if (i_BoneIDs[i] >= MAX_BONES) {
            pos = vec4(i_Pos, 1.0);
            break;
        }
        
        vec4 localPosition = finalBonesMatrices[i_BoneIDs[i]] * vec4(i_Pos, 1.0);
        pos += localPosition * i_BoneWeights[i];
        //vec3 localNormal = mat3(finalBonesMatrices[i_BoneIDs[i]]) * norm;
    }
    
    gl_Position = MATRIX_MVP * pos; //vec4(i_Pos, 1.0)
    
    o.v_WorldPos = (MATRIX_M * pos).xyz;
    //o.v_WorldNormal = (MATRIX_M * vec4(i_Normal, 0.0)).xyz;
    o.v_WorldNormal = (vec4(i_Normal, 0.0) * inverse(MATRIX_M)).xyz; // Correct for non-uniform scaled objects
    o.v_UV = i_UV;
}



#type fragment
#version 450 core
#include "assets/shaders/include/Matrix.glsl"
#include "assets/shaders/include/Lighting.glsl"

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
    vec3 albedo = texture(u_Albedo, i.v_UV).rgb;
    float metallic = 0.0; //texture(u_Metallic, i.v_UV).r;
    float roughness = 1.0; //texture(u_Roughness, i.v_UV).r;
    float ao = 1.0;
    
    vec3 normal = normalize(i.v_WorldNormal);
    
    o_Albedo = vec4(albedo, ao);
    o_Specular = vec4(metallic, roughness, 1.0, 1.0);
    o_Pos = vec4(i.v_WorldPos, 1.0);
    o_Normal = vec4(normal, 0.0);
}