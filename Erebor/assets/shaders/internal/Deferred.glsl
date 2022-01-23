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
#include "assets/shaders/include/Matrix.glsl"
#include "assets/shaders/include/Lighting.glsl"

struct v2f {
    vec2 v_UV;
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Color;

layout(binding = 3) uniform sampler2D u_Albedo;
layout(binding = 4) uniform sampler2D u_Specular;
layout(binding = 5) uniform sampler2D u_Pos;
layout(binding = 6) uniform sampler2D u_Normal;

void main() {
    // Surface values
    vec4 gBuffer0 = texture(u_Albedo, i.v_UV);
    vec4 gBuffer1 = texture(u_Specular, i.v_UV);
    vec4 gBuffer2 = texture(u_Pos, i.v_UV);
    vec4 gBuffer3 = texture(u_Normal, i.v_UV);
    
    vec3 albedo = gBuffer0.rgb;
    float ao = gBuffer0.a;
    
    float metallic = gBuffer1.r;
    float roughness = gBuffer1.g;
    
    vec3 worldPos = gBuffer2.xyz;
    
    vec3 worldNormal = gBuffer3.xyz;
    
    vec3 viewDir = getViewDir(worldPos);

    vec3 directLighting = BRDF_Direct(light, albedo, metallic, roughness, ao, viewDir, worldPos, worldNormal);

    vec3 color = directLighting;


    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2)); 

    o_Color = vec4(color, 1.0);
    
    //o_Color = vec4(gBuffer3.rgb, 1.0);
}