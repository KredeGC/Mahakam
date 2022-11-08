#type vertex
#version 430 core
#include "internal/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;

struct v2f {
    vec3 v_Normal;
};

layout(location = 0) out v2f o;

void main() {
    mat4 invProjection = MATRIX_IP;
    mat3 invView = transpose(mat3(MATRIX_V));
    vec3 unprojected = (invProjection * vec4(i_Pos, 1.0)).xyz;
    vec3 viewDir = invView * unprojected;
    
    gl_Position = vec4(i_Pos.xy, 1.0, 1.0);
    o.v_Normal = viewDir;
}



#type fragment
#version 430 core

struct v2f {
    vec3 v_Normal;
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Color;

layout(binding = 0, location = 0) uniform samplerCube u_Environment;

void main() {
    vec3 normal = normalize(i.v_Normal);
    
    o_Color = texture(u_Environment, normal);
    
    //o_Color = vec4(1.0, 1.0, 0.0, 1.0);
}