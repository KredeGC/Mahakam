#type vertex
#version 330 core
#include "assets/shaders/include/Matrix.glsl"

layout(location = 0) in vec3 i_Pos;

out vec3 v_LocalPos;

void main()
{
    v_LocalPos = i_Pos;  
    gl_Position = MATRIX_MVP * vec4(i_Pos, 1.0);
}



#type fragment
#version 330 core
out vec4 o_Color;
in vec3 localPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = sampleSphericalMap(normalize(localPos)); // make sure to normalize localPos
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    o_Color = vec4(color, 1.0);
}