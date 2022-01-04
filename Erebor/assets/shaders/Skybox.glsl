#type vertex
#version 330 core
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;
layout(location = 2) in vec3 i_Normal;

out vec3 v_Normal;

void main() {
    mat4 rotView = mat4(mat3(MATRIX_V));
    gl_Position = (MATRIX_P * rotView * vec4(i_Pos, 1.0)).xyww;
    v_Normal = i_Normal;
}



#type fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec3 v_Normal;

uniform samplerCube u_Environment;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {
    //vec2 uv = sampleSphericalMap(v_Normal);
    o_Color = texture(u_Environment, normalize(v_Normal));
}