#type vertex
#version 430
#include "internal/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
}



#type fragment
#version 430

layout(location = 0) out vec4 o_Albedo;

void main() {
    o_Albedo = vec4(0.0, 1.0, 0.0, 1.0);
}