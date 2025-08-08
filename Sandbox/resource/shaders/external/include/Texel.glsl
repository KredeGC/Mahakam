vec3 SnapDeltaPos(vec2 originalUV, vec4 texelSize, vec3 originalWorldPos) {
    // 1.) Calculate how much the texture UV coords need to
    //     shift to be at the center of the nearest texel.
    vec2 centerUV = floor(originalUV * texelSize.zw) * texelSize.xy + (texelSize.xy * 0.5);
    vec2 dUV = (centerUV - originalUV);

    // 2a.) Calculate how much the texture coords vary over fragment space.
    //      This essentially defines a 2x2 matrix that gets
    //      texture space (UV) deltas from fragment space (ST) deltas
    // Note: I call fragment space (S,T) to disambiguate.
    vec2 dUVdS = dFdx(originalUV);
    vec2 dUVdT = dFdy(originalUV);

    // 2b.) Invert the fragment from texture matrix
    mat2 dSTdUV = transpose(mat2(dUVdT[1], -dUVdT[0], -dUVdS[1], dUVdS[0]) * (1.0 / (dUVdS[0] * dUVdT[1] - dUVdT[0] * dUVdS[1])));


    // 2c.) Convert the UV delta to a fragment space delta
    vec2 dST = dSTdUV * dUV;
    
    // 3a.) Calculate how much the world coords vary over fragment space.
    vec3 dXYZdS = dFdx(originalWorldPos);
    vec3 dXYZdT = dFdy(originalWorldPos);

    // 3b.) Finally, convert our fragment space delta to a world space delta
    // And be sure to clamp it to SOMETHING in case the derivative calc went insane
    // Here I clamp it to -1 to 1 unit in unity, which should be orders of magnitude greater
    // than the size of any texel.
    vec3 dXYZ = dXYZdS * dST.x + dXYZdT * dST.y;

    dXYZ = clamp(dXYZ, -0.5, 0.5);
    
    return dXYZ;
}