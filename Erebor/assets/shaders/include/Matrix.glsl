#define MATRIX_M u_m4_M
#define MATRIX_V u_m4_V
#define MATRIX_P u_m4_P
#define MATRIX_MVP MATRIX_P * MATRIX_V * MATRIX_M

// IDEA: Use SSBO with offset etc.
uniform mat4 u_m4_M;

layout (std140) uniform Matrices {
    mat4 u_m4_V;
    mat4 u_m4_P;
    vec3 u_CameraPos;
};

vec3 getViewDir(vec3 worldPos) {
    if (MATRIX_P[3][3] > 0)
        return normalize(inverse(MATRIX_V)[2].xyz);
    else
        return normalize(u_CameraPos - worldPos);
}