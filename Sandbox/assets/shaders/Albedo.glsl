#type vertex
#version 330
#include "assets/shaders/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;

out vec3 v_WorldNormal;
out vec2 v_UV;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
    v_UV = i_UV;
    v_WorldNormal = (MATRIX_MVP * vec4(i_Normal, 0.0)).xyz;
}


#type fragment

#version 330

layout(location = 0) out vec4 color;

in vec3 v_WorldNormal;
in vec2 v_UV;

uniform sampler2D u_Albedo;

void main() {
    vec3 worldNormal = normalize(v_WorldNormal);
    //color = vec4(v_UV, 0.0, 1.0);
    
    vec4 albedo = texture(u_Albedo, v_UV);
    float NdotL = dot(worldNormal, vec3(0.0, 0.0, -1.0));

    color = albedo * NdotL;
}