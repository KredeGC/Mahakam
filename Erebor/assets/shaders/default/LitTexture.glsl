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

layout(location = 5) uniform vec4 u_UVTransform;

void main() {
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
    
    o.v_WorldPos = (MATRIX_M * vec4(i_Pos, 1.0)).xyz;
    //o.v_WorldNormal = (MATRIX_M * vec4(i_Normal, 0.0)).xyz;
    o.v_WorldNormal = (vec4(i_Normal, 0.0) * inverse(MATRIX_M)).xyz; // Correct for non-uniform scaled objects
    o.v_WorldTangent = (MATRIX_M * vec4(i_Tangent, 0.0)).xyz;
    o.v_WorldBinormal = cross(o.v_WorldNormal, o.v_WorldTangent);
    o.v_UV = u_UVTransform.xy * i_UV + u_UVTransform.zw;
}



#type fragment
#version 430 core
#include "assets/shaders/include/Matrix.glsl"
#include "assets/shaders/include/Normal.glsl"

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

layout(binding = 0, location = 0) uniform sampler2D u_Albedo;
layout(binding = 1, location = 1) uniform sampler2D u_Bump;
layout(binding = 2, location = 2) uniform sampler2D u_Metallic;
layout(binding = 3, location = 3) uniform sampler2D u_Roughness;
layout(binding = 4, location = 4) uniform sampler2D u_Occlussion;

void main() {
    // Surface values
    vec3 albedo = texture(u_Albedo, i.v_UV).rgb;
    vec3 bump = UnpackNormal(texture(u_Bump, i.v_UV).xy);
    float metallic = texture(u_Metallic, i.v_UV).r;
    float roughness = texture(u_Roughness, i.v_UV).r;
    float ao = texture(u_Occlussion, i.v_UV).r;
    
    //albedo = pow(albedo, vec3(2.2)); // sRGB correction
    
    //vec3 normal = normalize(i.v_WorldNormal);
    
    mat3 tbn = mat3(normalize(i.v_WorldTangent), normalize(i.v_WorldBinormal), normalize(i.v_WorldNormal));
    
    vec3 normal = normalize(tbn * bump);
    
    o_Albedo = vec4(albedo, ao);
    o_Specular = vec4(0.0, 0.0, metallic, roughness);
    o_Emission = vec4(i.v_WorldPos, 1.0);
    o_Normal = vec4(normal * 0.5 + 0.5, 0.0);
}