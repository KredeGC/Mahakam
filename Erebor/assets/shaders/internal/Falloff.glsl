#type vertex
#version 430 core

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;

out vec2 v_UV;

void main() {
    v_UV = i_UV;
    gl_Position = vec4(i_Pos, 1.0);
}



#type fragment
#version 430 core

layout(location = 0) out vec4 o_Color;

in vec2 v_UV;

float calculateFalloff(vec2 uv) {
    float normalizedDist = uv.x * uv.y;
    
    //return 1.0 / (1.0 + 25.0 * normalizedDist * normalizedDist);
    
    //return clamp(1.0 / (1.0 + 25.0 * normalizedDist * normalizedDist) * clamp((1.0 - normalizedDist) * 5.0, 0.0, 1.0), 0.0, 1.0);
    
    return pow(clamp(1.0 - normalizedDist * normalizedDist, 0.0, 1.0), 2.0);
}

void main() {
    float falloff = calculateFalloff(v_UV);
    o_Color = vec4(falloff, 1.0, 1.0, 1.0);
}