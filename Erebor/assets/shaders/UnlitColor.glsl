#type vertex
#version 430
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;

layout (std140, binding = UNIFORM_BINDING) uniform Uniforms {
    vec3 u_Color;
};

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
}



#type fragment
#version 430
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) out vec4 o_Albedo;

layout (std140, binding = UNIFORM_BINDING) uniform Uniforms {
    vec3 u_Color;
};

void main() {
    o_Albedo = vec4(u_Color, 1.0);
}