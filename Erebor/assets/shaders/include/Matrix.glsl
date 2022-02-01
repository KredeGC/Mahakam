#ifndef MATRIX_INCLUDED
#define MATRIX_INCLUDED

#define MATRIX_M u_m4_M
#define MATRIX_V u_m4_V
#define MATRIX_P u_m4_P
#define MATRIX_IV u_m4_IV
#define MATRIX_IP u_m4_IP
#define MATRIX_VP u_m4_VP
#define MATRIX_IVP u_m4_IVP
#define MATRIX_MVP MATRIX_VP * MATRIX_M

// IDEA: Use SSBO with offset etc. Would take up too much space :(
layout(location = 200) uniform mat4 u_m4_M;

layout (std140, binding = 0) uniform Matrices {
    mat4 u_m4_V;
    mat4 u_m4_P;
    
    mat4 u_m4_IV;
    mat4 u_m4_IP;
    
    mat4 u_m4_VP;
    mat4 u_m4_IVP;
    
    vec3 u_CameraPos;
};

vec3 getViewDir(vec3 worldPos) {
    if (MATRIX_P[3][3] > 0)
        return normalize(MATRIX_IV[2].xyz);
    else
        return normalize(u_CameraPos - worldPos);
}

#endif