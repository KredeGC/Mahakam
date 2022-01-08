#type vertex
#version 330
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;

out vec3 v_WorldPos;
out vec3 v_WorldNormal;
out vec3 v_ViewDir;
out vec2 v_UV;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
    v_WorldPos = (MATRIX_M * vec4(i_Pos, 1.0)).xyz;
    //v_WorldNormal = (MATRIX_M * vec4(i_Normal, 0.0)).xyz;
    v_WorldNormal = (vec4(i_Normal, 0.0) * inverse(MATRIX_M)).xyz; // Correct for non-uniform scaled objects
    v_UV = i_UV;
    v_ViewDir = getViewDir(v_WorldPos);
}



#type fragment
#version 330
#include "assets/shaders/include/Matrix.glsl"
#include "assets/shaders/include/Lighting.glsl"

layout(location = 0) out vec4 o_Color;

in vec3 v_WorldPos;
in vec3 v_WorldNormal;
in vec3 v_ViewDir;
in vec2 v_UV;

uniform sampler2D u_Albedo;
uniform sampler2D u_Metallic;
uniform sampler2D u_Roughness;
uniform float u_AO;

void main() {
    // Surface values
    vec3 albedo = texture(u_Albedo, v_UV).rgb;
    float metallic = texture(u_Metallic, v_UV).r;
    float roughness = texture(u_Roughness, v_UV).r;
    float ao = 1.0;

    vec3 viewdir = normalize(v_ViewDir);
    vec3 normal = normalize(v_WorldNormal);

    vec3 directLighting = BRDF_Direct(light, albedo, metallic, roughness, ao, viewdir, v_WorldPos, normal);

    vec3 color = directLighting;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2)); 

    o_Color = vec4(color, 1.0);
}