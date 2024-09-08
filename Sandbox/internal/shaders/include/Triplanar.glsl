#ifndef TRIPLANAR_INCLUDED
#define TRIPLANAR_INCLUDED

#define TRIPLANAR_UV_OFFSET
#define TRIPLANAR_CORRECT_PROJECTED_U

vec3 TriplanarWeights(vec3 normal) {
    vec3 triblend = pow(abs(normal), vec3(4.0));
    vec3 triW = clamp(triblend, 0.0, 1.0);
    return triW / (triW.x + triW.y + triW.z);
}

vec3 CalculateAxisSign(vec3 normal) {
    return sign(normal);
    
    // // Minor optimization of sign(). prevents return value of 0
    // vec3 axisSign = vec3(
    //     normal.x < 0.0 ? -1.0 : 1.0,
    //     normal.y < 0.0 ? -1.0 : 1.0,
    //     normal.z < 0.0 ? -1.0 : 1.0
    // );
    
    // return axisSign;
}

void CreateTriplanarUVs(vec3 pos, vec3 normal, vec3 axisSign, vec4 transform, out vec2 uvX, out vec2 uvY, out vec2 uvZ) {
    vec2 offset = transform.zw;
    vec2 scale = transform.xy;
    uvX = offset + pos.zy * scale;
    uvY = offset + pos.xz * scale;
    uvZ = offset + pos.xy * scale;
    
    // Offset UVs to prevent obvious mirroring
    #ifdef TRIPLANAR_UV_OFFSET
        uvY += 0.33;
        uvZ += 0.67;
    #endif
    
    // Flip UVs horizontally to correct for back side projection
    #ifdef TRIPLANAR_CORRECT_PROJECTED_U
        uvX.x *= axisSign.x;
        uvY.x *= axisSign.y;
        uvZ.x *= -axisSign.z;
    #endif
}

vec4 SampleTriplanar(sampler2D tex, vec2 uvX, vec2 uvY, vec2 uvZ, vec3 blend) {
    vec4 albedoX = SAMPLE_TEXTURE(tex, uvX);
    vec4 albedoY = SAMPLE_TEXTURE(tex, uvY);
    vec4 albedoZ = SAMPLE_TEXTURE(tex, uvZ);
    
    return albedoX * blend.x + albedoY * blend.y + albedoZ * blend.z;
}

// vec3 blend_rnm(vec3 n1, vec3 n2) {
//     n1.z += 1;
//     n2.xy = -n2.xy;

//     return n1 * dot(n1, n2) / n1.z - n2;
// }

vec3 blend_rnm(vec3 n1, vec3 n2) {
    //n1.z += 1;
    //n2.xy = -n2.xy;
    //return n1 * dot(n1, n2) / n1.z - n2;
    n1 += vec3(0, 0, 1);
    n2 += vec3(0, 0, 1);
    vec3 n = n1*dot(n1, n2)/n1.z - n2;
    if (n.z < 0) n = normalize(vec3(n.x, n.y, 0));
    return n;
}

vec3 SampleTriplanarNormal(sampler2D tex, vec2 uvX, vec2 uvY, vec2 uvZ, vec3 normal, vec3 axisSign, vec3 blend) {
    // tangent space normal maps
    vec3 tnormalX = UnpackNormal(SAMPLE_TEXTURE(tex, uvX).xy);
    vec3 tnormalY = UnpackNormal(SAMPLE_TEXTURE(tex, uvY).xy);
    vec3 tnormalZ = UnpackNormal(SAMPLE_TEXTURE(tex, uvZ).xy);
    
    // flip normal maps' x axis to account for flipped UVs
    #ifdef TRIPLANAR_CORRECT_PROJECTED_U
        tnormalX.x *= axisSign.x;
        tnormalY.x *= axisSign.y;
        tnormalZ.x *= -axisSign.z;
    #endif
    
    vec3 absVertNormal = abs(normal);
    
    // swizzle world normals to match tangent space and apply reoriented normal mapping blend
    tnormalX = blend_rnm(vec3(normal.zy, absVertNormal.x), tnormalX);
    tnormalY = blend_rnm(vec3(normal.xz, absVertNormal.y), tnormalY);
    tnormalZ = blend_rnm(vec3(normal.xy, absVertNormal.z), tnormalZ);
    
    // apply world space sign to tangent space Z
    tnormalX.z *= axisSign.x;
    tnormalY.z *= axisSign.y;
    tnormalZ.z *= axisSign.z;
    
    // swizzle tangent normals to match world normal and blend together
    vec3 swizzleNormal = normalize(
        tnormalX.zyx * blend.x +
        tnormalY.xzy * blend.y +
        tnormalZ.xyz * blend.z
    );
    
    return swizzleNormal;
}

#endif // TRIPLANAR_INCLUDED