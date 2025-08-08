#type vertex
#version 430
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;

out vec2 v_UV;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
    v_UV = i_UV;
}



#type fragment
#version 430

layout(location = 0) out vec4 o_Color;

in vec2 v_UV;

void main() {
    o_Color = vec4(v_UV, 0.0, 1.0);
}