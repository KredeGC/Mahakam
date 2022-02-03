#type vertex
#version 450 core
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
}



#type fragment
#version 450 core

void main() {}