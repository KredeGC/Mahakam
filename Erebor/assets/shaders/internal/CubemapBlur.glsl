#type vertex
#version 450 core

layout(location = 0) in vec3 i_Pos;

layout(location = 0) out vec3 v_LocalPos;

layout(location = 1) uniform mat4 projection;
layout(location = 2) uniform mat4 view;

void main() {
    v_LocalPos = i_Pos;  
    gl_Position = projection * view * vec4(i_Pos, 1.0);
}



#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec3 v_LocalPos;

layout(binding = 0, location = 0) uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main() {
    vec3 normal = normalize(v_LocalPos);
    //vec2 uv = sampleSphericalMap(normal);
    //vec3 irradiance = texture(equirectangularMap, uv).rgb;

    // if (irradiance)
    vec3 irradiance = vec3(0.0);

    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up         = normalize(cross(normal, right));

    const float sampleDelta = 0.01;
    float nrSamples = (2.0 * PI) / sampleDelta * (0.5 * PI) / sampleDelta; // 0.0;
    float invNrSamples = 1.0 / nrSamples;
    
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += PI * invNrSamples * min(textureLod(environmentMap, sampleVec, 0.0).rgb, 1000.0) * cos(theta) * sin(theta);
            //nrSamples++;
        }
    }
    //irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    o_Color = vec4(irradiance, 1.0);
}