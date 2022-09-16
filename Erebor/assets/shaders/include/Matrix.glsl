#ifndef MATRIX_INCLUDED
#define MATRIX_INCLUDED

#include "assets/shaders/include/Binding.glsl"

#define MATRIX_M u_m4_M
#define MATRIX_V u_m4_V
#define MATRIX_P u_m4_P
#define MATRIX_IV u_m4_IV
#define MATRIX_IP u_m4_IP
#define MATRIX_VP u_m4_VP
#define MATRIX_IVP u_m4_IVP
#define MATRIX_MV MATRIX_V * MATRIX_P
#define MATRIX_MVP MATRIX_VP * MATRIX_M

// IDEA: Use SSBO with offset etc. Would take up too much space :(
layout(location = 200) uniform mat4 u_m4_M;

layout(std140, binding = MATRICES_BINDING) uniform Matrices {
    mat4 u_m4_V;
    mat4 u_m4_P;
    
    mat4 u_m4_IV;
    mat4 u_m4_IP;
    
    mat4 u_m4_VP;
    mat4 u_m4_IVP;
    
    vec3 u_CameraPos;
    
    vec4 u_ScreenParams;
};

vec3 getViewDir(vec3 worldPos) {
    if (MATRIX_P[3][3] > 0)
        return normalize(MATRIX_IV[2].xyz);
    else
        return normalize(u_CameraPos - worldPos);
}

vec3 transformWorldPos(mat4 modelMatrix, vec3 pos) {
    return (modelMatrix * vec4(pos, 1.0)).xyz;
}

vec3 transformWorldNormal(mat4 modelMatrix, vec3 normal) {
    //return (modelMatrix * vec4(normal, 0.0)).xyz; // Correct for uniformly scaled objects
    return (vec4(normal, 0.0) * inverse(modelMatrix)).xyz; // Correct for non-uniform scaled objects
}

vec3 transformWorldTangent(mat4 modelMatrix, vec3 tangent) {
    return (modelMatrix * vec4(tangent, 0.0)).xyz;
}

vec3 transformWorldBinormal(vec3 worldNormal, vec3 worldTangent, float tangentSign) {
    return cross(worldNormal, worldTangent) * tangentSign;
}

vec2 transformTexCoordinates(vec2 coords, vec4 scaleOffset) {
    return scaleOffset.xy * coords + scaleOffset.zw;
}

#endif // MATRIX_INCLUDED