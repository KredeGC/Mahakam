#type vertex
#version 450 core
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec3 v_ViewDir;
    vec2 v_UV;
};

layout(location = 0) out v2f o;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
    
    o.v_WorldPos = (MATRIX_M * vec4(i_Pos, 1.0)).xyz;
    //o.v_WorldNormal = (MATRIX_M * vec4(i_Normal, 0.0)).xyz;
    o.v_WorldNormal = (vec4(i_Normal, 0.0) * inverse(MATRIX_M)).xyz; // Correct for non-uniform scaled objects
    o.v_UV = i_UV;
    o.v_ViewDir = getViewDir(o.v_WorldPos);
}



#type fragment
#version 450 core
#include "assets/shaders/include/Matrix.glsl"
#include "assets/shaders/include/Lighting.glsl"

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec3 v_ViewDir;
    vec2 v_UV;
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Color;

layout(binding = 3) uniform sampler2D u_Albedo;
layout(binding = 4) uniform sampler2D u_Metallic;
layout(binding = 5) uniform sampler2D u_Roughness;
layout(location = 0) uniform float u_AO; // ??

void main() {
    // Surface values
    vec3 albedo = texture(u_Albedo, i.v_UV).rgb;
    float metallic = texture(u_Metallic, i.v_UV).r;
    float roughness = texture(u_Roughness, i.v_UV).r;
    float ao = 1.0;

    vec3 viewdir = normalize(i.v_ViewDir);
    vec3 normal = normalize(i.v_WorldNormal);

    vec3 directLighting = BRDF_Direct(light, albedo, metallic, roughness, ao, viewdir, i.v_WorldPos, normal);

    vec3 color = directLighting;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2)); 

    o_Color = vec4(color, 1.0);
}