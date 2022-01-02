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

    // if (irradiance)
    vec3 irradiance = vec3(0.0);

    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up         = normalize(cross(normal, right));

    float sampleDelta = 0.01;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            vec2 uv = sampleSphericalMap(normalize(sampleVec));
            irradiance += texture(equirectangularMap, uv).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    o_Color = vec4(irradiance, 1.0);
}