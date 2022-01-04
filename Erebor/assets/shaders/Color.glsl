#type vertex
#version 330
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
}



#type fragment
#version 330

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main() {
    color = u_Color;
}