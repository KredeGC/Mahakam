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

layout(binding = 0, location = 0) uniform sampler2D u_Albedo;

// Needs gamma correction
vec3 reinhard(vec3 x) {
    return x / (x + vec3(1.0));
}

// Needs gamma correction
vec3 variableExposure(vec3 x, float exposure) {
    return vec3(1.0) - exp(-x * exposure);
}

// Needs gamma correction
vec3 aces(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// Has built-in gamma correction
vec3 unreal(vec3 x) {
    return x / (x + 0.155) * 1.019;
}

void main() {
    vec3 color = texture(u_Albedo, i.v_UV).rgb;
    
    // HDR tonemapping
    //color = reinhard(color);
    //const float exposure = 3.0;
    //color = variableExposure(color, exposure);
    //color = aces(color);
    color = unreal(color);
    
    // Gamma correction
    //color = pow(color, vec3(0.45454545)); 

    o_Color = vec4(color, 1.0);
}