#type vertex
#version 430 core
#include "assets/shaders/include/Matrix.glsl"

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec3 v_WorldTangent;
    vec3 v_WorldBinormal;
    vec2 v_UV;
};

layout(location = 0) out v2f o;

layout(location = 0) in vec3 i_Pos;
layout(location = 1) in vec2 i_UV;
layout(location = 2) in vec3 i_Normal;
layout(location = 3) in vec3 i_Tangent;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
    
    o.v_WorldPos = (MATRIX_M * vec4(i_Pos, 1.0)).xyz;
    //o.v_WorldNormal = (MATRIX_M * vec4(i_Normal, 0.0)).xyz;
    o.v_WorldNormal = (vec4(i_Normal, 0.0) * inverse(MATRIX_M)).xyz; // Correct for non-uniform scaled objects
    o.v_WorldTangent = (MATRIX_M * vec4(i_Tangent, 0.0)).xyz;
    o.v_WorldBinormal = cross(o.v_WorldNormal, o.v_WorldTangent);
    o.v_UV = i_UV;
}



#type fragment
#version 430 core
#include "assets/shaders/include/Matrix.glsl"

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec3 v_WorldTangent;
    vec3 v_WorldBinormal;
    vec2 v_UV;
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Specular;
layout(location = 2) out vec4 o_Emission;
layout(location = 3) out vec4 o_Normal;
layout(location = 4) out vec4 o_DeltaPos;

layout(binding = 0) uniform sampler2D u_Albedo;
layout(binding = 1) uniform sampler2D u_Metallic;
layout(binding = 2) uniform sampler2D u_Roughness;
uniform float u_AO; // ??

vec3 snapWorldPos(vec2 originalUV, vec4 texelSize, vec3 originalWorldPos) {
    // 1.) Calculate how much the texture UV coords need to
    //     shift to be at the center of the nearest texel.
    vec2 centerUV = floor(originalUV * texelSize.zw) * texelSize.xy + (texelSize.xy * 0.5);
    vec2 dUV = (centerUV - originalUV);

    // 2a.) Calculate how much the texture coords vary over fragment space.
    //      This essentially defines a 2x2 matrix that gets
    //      texture space (UV) deltas from fragment space (ST) deltas
    // Note: I call fragment space (S,T) to disambiguate.
    vec2 dUVdS = dFdx(originalUV);
    vec2 dUVdT = dFdy(originalUV);

    // 2b.) Invert the fragment from texture matrix
    mat2 dSTdUV = transpose(mat2(dUVdT[1], -dUVdT[0], -dUVdS[1], dUVdS[0]) * (1.0 / (dUVdS[0] * dUVdT[1] - dUVdT[0] * dUVdS[1])));


    // 2c.) Convert the UV delta to a fragment space delta
    vec2 dST = dSTdUV * dUV;
    
    // 3a.) Calculate how much the world coords vary over fragment space.
    vec3 dXYZdS = dFdx(originalWorldPos);
    vec3 dXYZdT = dFdy(originalWorldPos);

    // 3b.) Finally, convert our fragment space delta to a world space delta
    // And be sure to clamp it to SOMETHING in case the derivative calc went insane
    // Here I clamp it to -1 to 1 unit in unity, which should be orders of magnitude greater
    // than the size of any texel.
    vec3 dXYZ = dXYZdS * dST.x + dXYZdT * dST.y;

    dXYZ = clamp(dXYZ, -1.0, 1.0);
    
    return dXYZ;
}

void main() {
    // Surface values
    vec3 albedo = texture(u_Albedo, i.v_UV).rgb;
    float metallic = texture(u_Metallic, i.v_UV).r;
    float roughness = texture(u_Roughness, i.v_UV).r;
    float ao = 1.0;
    
    albedo = pow(albedo, vec3(2.2)); // sRGB correction
    
    vec3 normal = normalize(i.v_WorldNormal);
    
    // Calculate per texel position snap
    vec4 texelSize = vec4(1.0 / 128.0, 1.0 / 128.0, 128.0, 128.0);
    vec3 deltaPos = snapWorldPos(i.v_UV, texelSize, i.v_WorldPos);
    
    o_Albedo = vec4(albedo, ao);
    o_Specular = vec4(0.0, 0.0, metallic, roughness);
    o_Emission = vec4(i.v_WorldPos, 1.0);
    o_Normal = vec4(normal * 0.5 + 0.5, 0.0);
    o_DeltaPos = vec4(deltaPos, 0.0);
}