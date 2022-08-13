#include "Mahakam/mhpch.h"
#include "Animation.h"

#include "Assimp.h"

#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam
{
    Asset<Animation> Animation::Load(const std::string& filepath, SkinnedMesh& skinnedMesh)
    {
        return Asset<Animation>(CreateRef<Animation>(filepath, skinnedMesh));
    }

    Animation::Animation(const std::string& filepath, SkinnedMesh& skinnedMesh)
        : m_Filepath(filepath)
    {
        MH_PROFILE_FUNCTION();

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            MH_CORE_WARN("Could not load animation \"{0}\": {1}", filepath, importer.GetErrorString());
        }
        else
        {
            auto animation = scene->mAnimations[0];
            m_Name = animation->mName.C_Str();
            m_Duration = (float)animation->mDuration;
            m_TicksPerSecond = (int)animation->mTicksPerSecond;

            m_BoneInfoMap = skinnedMesh.boneInfo;

            ReadHierarchyData(-1, scene->mRootNode);
        }
    }

    void Animation::ReadHierarchyData(int parentIndex, const aiNode* src)
    {
        MH_PROFILE_FUNCTION();

        MH_CORE_ASSERT(src, "Invalid bone!");

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