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
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
    o.v_WorldPos = (MATRIX_M * vec4(i_Pos, 1.0)).xyz;
    o.v_WorldNormal = (vec4(i_Normal, 0.0) * inverse(MATRIX_M)).xyz; // Correct for non-uniform scaled objects
    o.v_UV = i_UV;
}



#type fragment
#version 430
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Specular;
layout(location = 2) out vec4 o_Emission;
layout(location = 3) out vec4 o_Normal;

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec2 v_UV;
};

layout(location = 0) in v2f i;

layout(location = 1) uniform vec3 u_Color;
layout(location = 2) uniform float u_Metallic;
layout(location = 3) uniform float u_Roughness;
layout(location = 4) uniform vec3 u_Emission;

void main() {
    // Get normal from vertices
    vec3 normal = normalize(i.v_WorldNormal);
    
    o_Albedo = vec4(u_Color, 1.0);
    o_Specular = vec4(0.0, 0.0, u_Metallic, u_Roughness);
    o_Emission = vec4(u_Emission, 0.0);
    o_Normal = vec4(normal * 0.5 + 0.5, 0.0);
}