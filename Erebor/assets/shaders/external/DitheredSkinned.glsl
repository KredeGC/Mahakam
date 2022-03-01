#type vertex
#version 450 core
#include "assets/shaders/include/Matrix.glsl"

#if defined(GEOMETRY)
    struct v2f {
        vec4 v_ClipPos;
        vec3 v_WorldPos;
        vec3 v_WorldNormal;
        vec2 v_UV;
    };

    layout(location = 0) out v2f o;
#elif defined(SHADOW)
    layout (std140, binding = 1) uniform LightMatrices {
        mat4 u_WorldToLight;
    };
#endif

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;
layout(location = 3) in vec3 i_Tangent;
layout(location = 5) in ivec4 i_BoneIDs;
layout(location = 6) in vec4 i_BoneWeights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout(location = 6) uniform mat4 finalBonesMatrices[MAX_BONES];

void main() {
    // vec4 boneWeights = i_BoneWeights;
    // float size = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;
    // boneWeights = boneWeights / size;
    
    mat4 boneTransform = finalBonesMatrices[i_BoneIDs[0]] * i_BoneWeights[0];
    boneTransform += finalBonesMatrices[i_BoneIDs[1]] * i_BoneWeights[1];
    boneTransform += finalBonesMatrices[i_BoneIDs[2]] * i_BoneWeights[2];
    boneTransform += finalBonesMatrices[i_BoneIDs[3]] * i_BoneWeights[3];
    
    //mat3 normalMatrix = transpose(inverse(mat3(boneTransform)));
    vec4 pos = boneTransform * vec4(i_Pos, 1.0);
    
    #if defined(GEOMETRY)
        vec3 normal = mat3(boneTransform) * i_Normal;
        vec3 tangent = mat3(boneTransform) * i_Tangent;
        
        gl_Position = MATRIX_MVP * pos; //vec4(i_Pos, 1.0)
        
        o.v_ClipPos = gl_Position;
        o.v_WorldPos = (MATRIX_M * pos).xyz;
        //o.v_WorldNormal = (MATRIX_M * vec4(normal, 0.0)).xyz;
        o.v_WorldNormal = (normal * inverse(mat3(MATRIX_M))).xyz; // Correct for non-uniform scaled objects
        o.v_UV = i_UV;
    #elif defined(SHADOW)
        gl_Position = u_WorldToLight * MATRIX_M * pos;
    #endif
}



#type fragment
#version 450 core
#include "assets/shaders/include/Matrix.glsl"
#include "assets/shaders/include/Normal.glsl"

#if defined(GEOMETRY)
    struct v2f {
        vec4 v_ClipPos;
        vec3 v_WorldPos;
        vec3 v_WorldNormal;
        vec2 v_UV;
    };

    layout(location = 0) in v2f i;
#endif

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Specular;
layout(location = 2) out vec4 o_Emission;
layout(location = 3) out vec4 o_Normal;
layout(location = 4) out vec4 o_DeltaPos;

layout(location = 0) uniform vec3 u_Color;
layout(location = 1) uniform float u_Metallic;
layout(location = 2) uniform float u_Roughness;

#define ROUNDING_PREC 0.999
#define PIXELSIZE 5.0
float ditherPixel(vec4 posCS, float alpha_in) {
    alpha_in = clamp(round(alpha_in), 0.0, 1.0);
    float xfactor = step(mod(abs(floor(posCS.x)), PIXELSIZE), ROUNDING_PREC);
    float yfactor = step(mod(abs(floor(posCS.y)), PIXELSIZE), ROUNDING_PREC);
    return alpha_in * xfactor * yfactor * alpha_in;
}

void main() {
#if defined(GEOMETRY)
    // Surface values
    vec3 albedo = u_Color;
    float metallic = u_Metallic;
    float roughness = u_Roughness;
    
    vec3 normal = normalize(i.v_WorldNormal);
    
    vec4 clipcs = i.v_ClipPos / i.v_ClipPos.w;
    clipcs += 1.0;
    clipcs.xy *= 0.5 * u_ScreenParams.xy;
    //vec4 clipcs = vec4(gl_FragCoord.xy)
    if (ditherPixel(clipcs, 1.0) < 1.0)
        discard;
    
    o_Albedo = vec4(albedo, 1.0);
    o_Specular = vec4(0.0, 0.0, metallic, roughness);
    o_Emission = vec4(i.v_WorldPos, 1.0);
    o_Normal = vec4(normal * 0.5 + 0.5, 0.0);
    o_DeltaPos = vec4(0.5, 0.5, 0.5, 0.0);
#endif
}