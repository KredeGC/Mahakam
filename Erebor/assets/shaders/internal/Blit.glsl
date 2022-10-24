#type vertex
#version 430 core

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;

struct v2f {
    vec2 v_UV;
};

layout(location = 0) out v2f o;

void main() {
    gl_Position = vec4(i_Pos.xy, 0.5, 1.0);
    o.v_UV = i_UV;
}



#type fragment
#version 430 core
#include "assets/shaders/include/Binding.glsl"

struct v2f {
    vec2 v_UV;
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Color;

layout(binding = 0, location = 0) uniform sampler2D u_Albedo;

layout(location = 1) uniform int u_Depth;

void main() {
    vec3 color = texture(u_Albedo, i.v_UV).rgb;
    
    if (u_Depth > 0)
        o_Color = vec4(color.r, color.r, color.r, 1.0);
    else
        o_Color = vec4(color, 1.0);
}