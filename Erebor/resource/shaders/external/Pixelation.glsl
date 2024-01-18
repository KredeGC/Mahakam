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

struct v2f {
    vec2 v_UV;
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Color;

layout(binding = 0, location = 0) uniform sampler2D u_Albedo;
layout(binding = 1, location = 1) uniform sampler2D u_Depth;

void main() {
    float depth;
    vec2 ppos;
    float raw_depth;
    
    vec2 pShift = 1.0 / textureSize(u_Albedo, 0).xy;
    
    float nearestDepth = 1.000001;
    vec4 nearestColor = vec4(0.0);

    // These limits determined by the pixel size
    for (int u = -2; u <= 2; u++) {
        for (int v = -2; v <= 2; v++) {
            //Get coord of neighbouring pixel for sampling
            float shiftx = u * pShift.x;
            float shifty = v * pShift.y;
            vec2 ppos = i.v_UV + vec2(shiftx, shifty);
            vec4 neighbour = texture(u_Albedo, ppos);
            depth = texture(u_Depth, ppos).r;
            
            // Check if the neighbouring pixel is nearest so far - if so, use its value
            bool nearer = (depth < nearestDepth);
            nearestDepth = nearer ? depth : nearestDepth;
            nearestColor = nearer ? neighbour : nearestColor;
        }
    }

    o_Color = nearestColor;
}