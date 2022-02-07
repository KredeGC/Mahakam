#type vertex
#version 450 core
#if defined(POINT) || defined(SPOT)
    layout(location = 0) out flat int v_InstanceID;
#endif

#include "assets/shaders/include/Matrix.glsl"
#include "assets/shaders/include/LightStruct.glsl"

layout(location = 0) in vec3 i_Pos;

struct v2f {
    vec3 v_ScreenPos;
};

layout(location = 1) out v2f o;

void main() {
    #if defined(POINT) || defined(SPOT)
        v_InstanceID = gl_InstanceID;
        #if defined(POINT)
            gl_Position = MATRIX_P * MATRIX_V * vec4(i_Pos * 2.0 * lights[v_InstanceID].position.w + lights[v_InstanceID].position.xyz, 1.0);
        #else
            gl_Position = MATRIX_P * MATRIX_V * lights[v_InstanceID].objectToWorld * vec4(i_Pos, 1.0);
        #endif
        o.v_ScreenPos = gl_Position.xyw;
    #else
        gl_Position = vec4(i_Pos, 1.0);
        o.v_ScreenPos = vec3(gl_Position.xy * 0.5 + 0.5, 1.0);
    #endif
}



#type fragment
#version 450 core
#if defined(POINT) || defined(SPOT)
    layout(location = 0) in flat int v_InstanceID;
#endif

#include "assets/shaders/include/Matrix.glsl"
#include "assets/shaders/include/Lighting.glsl"

struct v2f {
    vec3 v_ScreenPos;
};

layout(location = 1) in v2f i;

layout(location = 0) out vec4 o_Color;

layout(binding = 4, location = 4) uniform sampler2D u_GBuffer0;
layout(binding = 5, location = 5) uniform sampler2D u_GBuffer1;
layout(binding = 6, location = 6) uniform sampler2D u_GBuffer2;
layout(binding = 7, location = 7) uniform sampler2D u_Depth;
// layout(binding = 8) uniform sampler2D u_GBuffer3;

void main() {
    #if defined(POINT) || defined(SPOT)
        vec2 screenUV = (i.v_ScreenPos.xy / i.v_ScreenPos.z) * 0.5 + 0.5;
    #else
        vec2 screenUV = i.v_ScreenPos.xy;
    #endif
    
    #ifdef DEBUG
        screenUV *= 2.0;
    #endif
    
    // Surface values
    vec4 gBuffer0 = texture(u_GBuffer0, screenUV);
    vec4 gBuffer1 = texture(u_GBuffer1, screenUV);
    vec4 gBuffer2 = texture(u_GBuffer2, screenUV);
    //vec4 gBuffer3 = texture(u_GBuffer3, screenUV);
    float depth = texture(u_Depth, screenUV).r;
    
    // GBuffer0
    vec3 albedo = gBuffer0.rgb;
    float ao = gBuffer0.a;
    
    // GBuffer1
    float metallic = gBuffer1.b;
    float roughness = clamp(gBuffer1.a, 0.05, 1.0);
    
    // GBuffer2
    vec3 worldNormal = normalize(gBuffer2.xyz * 2.0 - 1.0);
    
    // Depth Buffer
    vec3 worldPos = depthToWorldSpace(screenUV, depth);
    
    // GBuffer3
    // vec3 worldPosOffset = gBuffer3.xyz;
    // worldPos += worldPosOffset;
    
    vec3 viewDir = getViewDir(worldPos);
    
    vec3 color = BRDF(albedo, metallic, roughness, ao, viewDir, worldPos, worldNormal);

    o_Color = vec4(color, 1.0);
    
    #ifdef DEBUG
        #ifdef DIRECTIONAL
            gBuffer0 = texture(u_GBuffer0, screenUV - vec2(1.0, 1.0));
            gBuffer1 = texture(u_GBuffer1, screenUV - vec2(1.0, 0.0));
            gBuffer2 = texture(u_GBuffer2, screenUV - vec2(0.0, 1.0));
            
            o_Color = vec4(0.0);
            o_Color += screenUV.x > 1.0 && screenUV.y > 1.0 ? gBuffer0 : vec4(0.0);
            o_Color += screenUV.x > 1.0 && screenUV.y < 1.0 ? gBuffer1 : vec4(0.0);
            o_Color += screenUV.x < 1.0 && screenUV.y > 1.0 ? gBuffer2 : vec4(0.0);
            o_Color += screenUV.x < 1.0 && screenUV.y < 1.0 ? vec4(color, 1.0) : vec4(0.0);
        #else
            o_Color = vec4(0.0);
        #endif
    #endif
    
    // #if defined(DIRECTIONAL)
    //     o_Color = vec4(0.0);
    // #else
    //     o_Color = vec4(1.0);
    // #endif
}