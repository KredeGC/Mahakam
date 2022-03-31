#include "mhpch.h"
#include "Animation.h"

#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam
{
    Ref<Animation> Animation::Load(const std::string& filepath, SkinnedMesh& skinnedMesh)
    {
        MH_OVERRIDE_FUNC(AnimationLoad, filepath, skinnedMesh);

        return CreateRef<Animation>(filepath, skinnedMesh);
    }

    Animation::Animation(const std::string& filepath, SkinnedMesh& skinnedMesh)
    {
        MH_PROFILE_FUNCTION();

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            MH_CORE_WARN("Could not load model \"{0}\": {1}", filepath, importer.GetErrorString());
        else
        {
            auto animation = scene->mAnimations[0];
            m_Duration = (float)animation->mDuration;
            m_TicksPerSecond = (int)animation->mTicksPerSecond;

            ReadHierarchyData(-1, scene->mRootNode);
            ReadMissingBones(animation, skinnedMesh.boneInfo, skinnedMesh.boneCount);
        }
    }

    Bone* Animation::FindBone(const std::string& name)
    {
        auto iter = m_Bones.find(name);
        if (iter != m_Bones.end())
            return &iter->second;

        return nullptr;
    }

    void Animation::ReadMissingBones(const aiAnimation* animation, robin_hood::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCount)
    {
        MH_PROFILE_FUNCTION();

        int size = animation->mNumChannels;

        // Reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            // If this bone wasn't loaded in the model
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }

            m_Bones[boneName] = Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel);
        }

        m_BoneInfoMap = boneInfoMap;
    }

    void Animation::ReadHierarchyData(int parentIndex, const aiNode* src)
    {
        MH_PROFILE_FUNCTION();

        MH_CORE_ASSERT(src, "Invalid root bone!");

        int index = (int)m_BoneHierarchy.size();

        BoneTransform dest;

        dest.name = src->mName.data;
        dest.transformation = AssimpToMat4(src->mTransformation);
        dest.parentIndex = parentIndex;

        m_BoneHierarchy.push_back(dest);

        for (unsigned int i = 0; i < src->mNumChildren; i++)
            ReadHierarchyData(index, src->mChildren[i]);
    }
}