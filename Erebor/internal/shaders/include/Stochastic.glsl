#ifndef STOCHASTIC_INCLUDED
#define STOCHASTIC_INCLUDED

#include "internal/shaders/include/Random.glsl"

vec4 SampleStochastic(sampler2D tex, vec2 uv) {
    // Triangle vertices and blend weights
    //bwvx[0...2].xyz = triangle verts
    //bwvx[3].xy = blend weights (z is unused)
    mat4 bwvx;

    // UV transformed into triangular grid space with uv scaled by approximation of 2*sqrt(3)
    vec2 skewUV = (uv * 3.464) * mat2(1.0, 0.0, -0.57735027, 1.15470054);

    // Vertex IDs and barycentric coords
    vec2 vxID = vec2(floor(skewUV));
    vec3 barry = vec3(fract(skewUV), 0.0);
    barry.z = 1.0 - barry.x - barry.y;
    
    // Choose projection
    mat4 first = mat4(
        vec4(vxID, 0.0, 0.0),
        vec4(vxID + vec2(0.0, 1.0), 0.0, 0.0),
        vec4(vxID + vec2(1.0, 0.0), 0.0, 0.0),
        vec4(barry.zyx, 0.0));
    mat4 second = mat4(
        vec4(vxID + vec2(1.0, 1.0), 0.0, 0.0),
        vec4(vxID + vec2(1.0, 0.0), 0.0, 0.0),
        vec4(vxID + vec2(0.0, 1.0), 0.0, 0.0),
        vec4(-barry.z, 1.0 - barry.y, 1.0 - barry.x, 0.0));
    bwvx = barry.z > 0.0 ? first : second;

    // Calculate derivatives to avoid triangular grid artifacts
    vec2 dx = dFdx(uv);
    vec2 dy = dFdy(uv);
    
    // Blend samples with calculated weights
    vec4 xComp = bwvx[3].x * textureGrad(tex, uv + rand2dTo2d(bwvx[0].xy), dx, dy);
    vec4 yComp = bwvx[3].y * textureGrad(tex, uv + rand2dTo2d(bwvx[1].xy), dx, dy);
    vec4 zComp = bwvx[3].z * textureGrad(tex, uv + rand2dTo2d(bwvx[2].xy), dx, dy);
    
    return xComp + yComp + zComp;
}

#endif // STOCHASTIC_INCLUDED