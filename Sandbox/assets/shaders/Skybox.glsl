#type vertex
#version 330
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;
layout(location = 2) in vec3 i_Normal;

out vec3 v_WorldNormal;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
    v_WorldNormal = (vec4(i_Normal, 0.0) * inverse(MATRIX_M)).xyz; // Correct for non-uniform scaled objects
}



#type fragment
#version 330

layout(location = 0) out vec4 o_Color;

in vec3 v_WorldNormal;

uniform sampler2D u_Albedo;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {
    vec3 normal = normalize(v_WorldNormal);
    vec2 uv = sampleSphericalMap(normal);
    
    o_Color = texture(u_Albedo, uv);
}