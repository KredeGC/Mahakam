#ifndef SKIN_INCLUDED
#define SKIN_INCLUDED

#include "internal/shaders/include/Binding.glsl"

#define MAX_BONES 100

// TODO: Find a better way than using the bones from the Uniforms buffer

mat4 transformBoneVertex(ivec4 boneIDs, vec4 boneWeights) {
    mat4 boneTransform = BoneMatrices[boneIDs[0]] * boneWeights[0];
    boneTransform += BoneMatrices[boneIDs[1]] * boneWeights[1];
    boneTransform += BoneMatrices[boneIDs[2]] * boneWeights[2];
    boneTransform += BoneMatrices[boneIDs[3]] * boneWeights[3];

    return boneTransform;
}

#endif // SKIN_INCLUDED