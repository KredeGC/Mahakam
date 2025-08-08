#type vertex
#version 430 core

layout(location = 0) in vec3 i_Pos;

struct v2f {
    vec3 v_LocalPos;
};

layout(location = 0) out v2f o;

layout(location = 1) uniform mat4 u_Projection;
layout(location = 2) uniform mat4 u_View;

void main() {
    o.v_LocalPos = i_Pos;  
    gl_Position = u_Projection * u_View * vec4(i_Pos, 1.0);
}



#type fragment
#version 430 core

struct v2f {
    vec3 v_LocalPos;
};

layout(location = 0) in v2f i;

layout(location = 0) out vec4 o_Color;

layout(binding = 0, location = 0) uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

const float PI = 3.14159265359;

void main() {
    vec3 normal = normalize(i.v_LocalPos);
    vec2 uv = sampleSphericalMap(normal);
    vec3 irradiance = texture(u_EquirectangularMap, uv).rgb;
    
    o_Color = vec4(irradiance, 1.0);
}