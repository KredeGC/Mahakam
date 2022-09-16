#ifndef SKIN_INCLUDED
#define SKIN_INCLUDED

#include "assets/shaders/include/Binding.glsl"

#define MAX_BONES 100

layout(std140, binding = 2) uniform SkinMatrices {
    mat4 u_Bones[MAX_BONES];
};

mat4 transformBoneVertex(ivec4 boneIDs, vec4 boneWeights) {
    mat4 boneTransform = u_Bones[boneIDs[0]] * boneWeights[0];
    boneTransform += u_Bones[boneIDs[1]] * boneWeights[1];
    boneTransform += u_Bones[boneIDs[2]] * boneWeights[2];
    boneTransform += u_Bones[boneIDs[3]] * boneWeights[3];

    return boneTransform;
}

#endif // SKIN_INCLUDED