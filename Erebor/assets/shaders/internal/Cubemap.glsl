#type vertex
#version 330 core

layout(location = 0) in vec3 i_Pos;

out vec3 v_LocalPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
    v_LocalPos = i_Pos;  
    gl_Position = projection * view * vec4(i_Pos, 1.0);
}



#type fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec3 v_LocalPos;

uniform sampler2D equirectangularMap;

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
    vec3 normal = normalize(v_LocalPos);
    vec2 uv = sampleSphericalMap(normal);
    vec3 irradiance = texture(equirectangularMap, uv).rgb;
    
    o_Color = vec4(irradiance, 1.0);
}