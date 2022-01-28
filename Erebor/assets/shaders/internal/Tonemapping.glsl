#type vertex
#version 450 core

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
#version 450 core

struct v2f {
    vec2 v_UV;
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D u_Albedo;

void main() {
    vec3 color = texture(u_Albedo, i.v_UV).rgb;
    
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(0.45454545)); 

    o_Color = vec4(color, 1.0);
}