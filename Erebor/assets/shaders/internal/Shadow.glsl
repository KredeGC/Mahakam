#type vertex
#version 430 core
#include "assets/shaders/include/Matrix.glsl"

layout (std140, binding = 1) uniform LightMatrices {
    mat4 u_WorldToLight;
};

layout(location = 0) in vec3 i_Pos;

void main() {
    gl_Position = u_WorldToLight * MATRIX_M * vec4(i_Pos, 1.0);
    
    // gl_Position.z = min(gl_Position.z, gl_Position.w); // Make sure it doesn't intersect the near-clipping plane
}



#type fragment
#version 430 core

void main() {}