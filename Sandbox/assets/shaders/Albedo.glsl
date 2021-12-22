#type vertex
#version 330

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;

uniform mat4 u_MVP;

out vec2 v_UV;

void main() {
    gl_Position = u_MVP * vec4(i_Pos, 1.0);
    v_UV = i_UV;
}


#type fragment

#version 330

layout(location = 0) out vec4 color;

in vec2 v_UV;

uniform sampler2D u_Albedo;

void main() {
    //color = vec4(v_UV, 0.0, 1.0);
    color = texture(u_Albedo, v_UV);
}