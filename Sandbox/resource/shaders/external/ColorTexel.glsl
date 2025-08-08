#type vertex
#version 430
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec2 v_UV;
};

layout(location = 0) out v2f o;

void main() {
    o.v_WorldPos = (MATRIX_M * vec4(i_Pos, 1.0)).xyz;
    o.v_WorldNormal = (vec4(i_Normal, 0.0) * inverse(MATRIX_M)).xyz; // Correct for non-uniform scaled objects
    o.v_UV = i_UV;
    
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
}



#type fragment
#version 430
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Specular;
layout(location = 2) out vec4 o_Pos;
layout(location = 3) out vec4 o_Normal;
layout(location = 4) out vec4 o_DeltaPos;

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec2 v_UV;
};

layout(location = 0) in v2f i;

layout(location = 1) uniform vec3 u_Color;
layout(location = 2) uniform float u_Metallic;
layout(location = 3) uniform float u_Roughness;
layout(location = 4) uniform float u_AO;

void main() {
    // Surface values
    vec3 albedo = u_Color;
    float metallic = u_Metallic;
    float roughness = u_Roughness;
    float ao = 1.0;
    
    vec3 normal = normalize(i.v_WorldNormal);
    
    o_Albedo = vec4(albedo, ao);
    o_Specular = vec4(0.0, 0.0, metallic, roughness);
    o_Pos = vec4(i.v_WorldPos, 1.0);
    o_Normal = vec4(normal * 0.5 + 0.5, 0.0);
    o_DeltaPos = vec4(0.5, 0.5, 0.5, 0.0);
}