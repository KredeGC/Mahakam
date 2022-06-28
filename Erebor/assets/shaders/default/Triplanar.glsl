#type vertex
#version 430 core
#include "assets/shaders/include/Matrix.glsl"

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec3 v_WorldTangent;
    vec3 v_WorldBinormal;
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
}



#type fragment
#version 430 core
#include "assets/shaders/include/Matrix.glsl"
#include "assets/shaders/include/Normal.glsl"

#ifdef USE_STOCHASTIC
#include "assets/shaders/include/Stochastic.glsl"
#define SAMPLE_TEXTURE SampleStochastic
#else
#define SAMPLE_TEXTURE texture
#endif

#include "assets/shaders/include/Triplanar.glsl"

struct v2f {
    vec3 v_WorldPos;
    vec3 v_WorldNormal;
    vec3 v_WorldTangent;
    vec3 v_WorldBinormal;
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Albedo;
layout(location = 1) out vec4 o_Specular;
layout(location = 2) out vec4 o_Emission;
layout(location = 3) out vec4 o_Normal;

layout(location = 0) uniform vec4 u_UVTransform;
layout(binding = 1, location = 1) uniform sampler2D u_Albedo;
layout(binding = 2, location = 2) uniform sampler2D u_Bump;
layout(binding = 3, location = 3) uniform sampler2D u_Metallic;
layout(binding = 4, location = 4) uniform sampler2D u_Roughness;
layout(binding = 5, location = 5) uniform sampler2D u_Occlussion;
layout(binding = 6, location = 6) uniform sampler2D u_Emission;
layout(location = 7) uniform vec3 u_EmissionColor;

void main() {
    // Triplanar calcs
    vec3 worldNormal = normalize(i.v_WorldNormal);
    vec3 axisSign = CalculateAxisSign(worldNormal);
    vec2 uvX, uvY, uvZ;
    CreateTriplanarUVs(i.v_WorldPos, worldNormal, axisSign, u_UVTransform, uvX, uvY, uvZ);
    vec3 blend = TriplanarWeights(worldNormal);
    
    // Surface values
    vec3 albedo = SampleTriplanar(u_Albedo, uvX, uvY, uvZ, blend).rgb;
    vec3 normal = SampleTriplanarNormal(u_Bump, uvX, uvY, uvZ, worldNormal, axisSign, blend);
    float metallic = SampleTriplanar(u_Metallic, uvX, uvY, uvZ, blend).r;
    float roughness = SampleTriplanar(u_Roughness, uvX, uvY, uvZ, blend).r;
    float ao = SampleTriplanar(u_Occlussion, uvX, uvY, uvZ, blend).r;
    vec3 emission = SampleTriplanar(u_Emission, uvX, uvY, uvZ, blend).rgb * u_EmissionColor;
    
    // o_Albedo = vec4(blend, 1.0);
    // o_Specular = vec4(0.0, 0.0, 0.0, 1.0);
    // o_Emission = vec4(0.0, 0.0, 0.0, 0.0);
    // o_Normal = vec4(worldNormal * 0.5 + 0.5, 0.0);
    
    o_Albedo = vec4(albedo, ao);
    o_Specular = vec4(0.0, 0.0, metallic, roughness);
    o_Emission = vec4(emission, 0.0);
    o_Normal = vec4(normal * 0.5 + 0.5, 0.0);
}