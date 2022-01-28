#pragma once

#include "Mahakam/Core/Core.h"

#include "Bone.h"
#include "Mesh.h"

#include <map>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>

namespace Mahakam
{
    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    class Animation
    {
    public:
        Animation() = default;

        // TODO: Make into a ref-counted asset
        Animation(const std::string& filepath, SkinnedMesh& skinnedMesh)
        {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate);
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
                MH_CORE_WARN("Could not load model \"{0}\": {1}", filepath, importer.GetErrorString());

            auto animation = scene->mAnimations[0];
            m_Duration = (float)animation->mDuration;
            m_TicksPerSecond = (int)animation->mTicksPerSecond;

            ReadHeirarchyData(m_RootNode, scene->mRootNode);
            ReadMissingBones(animation, skinnedMesh.boneInfo, skinnedMesh.boneCount);
        }

        Bone* FindBone(const std::string& name)
        {
            auto iter = m_Bones.find(name);
            if (iter != m_Bones.end())
            {
                return &iter->second;
            }

            return nullptr;
        }


        inline int GetTicksPerSecond() { return m_TicksPerSecond; }

        inline float GetDuration() { return m_Duration; }

        inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

        inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
        {
            return m_BoneInfoMap;
        }

        static Ref<Animation> load(const std::string& filepath, SkinnedMesh& skinnedMesh)
        {
            return CreateRef<Animation>(filepath, skinnedMesh);
        }

    private:
        void ReadMissingBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap, int& boneCount)
        {
            int size = animation->mNumChannels;

            //auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
            //int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

            //reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                std::string boneName = channel->mNodeName.data;

                if (boneInfoMap.find(boneName) == boneInfoMap.end())
                {
                    boneInfoMap[boneName].id = boneCount;
                    boneCount++;
                }

                m_Bones[boneName] = Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel);
            }

            m_BoneInfoMap = boneInfoMap;
        }

        void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
        {
            MH_CORE_ASSERT(src, "Invalid root!");

            dest.name = src->mName.data;
            dest.transformation = assimpToMat4(src->mTransformation);
            dest.childrenCount = src->mNumChildren;

            for (unsigned int i = 0; i < src->mNumChildren; i++)
            {
                AssimpNodeData newData;
                ReadHeirarchyData(newData, src->mChildren[i]);
                dest.children.push_back(newData);
            }
        }

        float m_Duration;
        int m_TicksPerSecond;
        std::unordered_map<std::string, Bone> m_Bones;
        AssimpNodeData m_RootNode;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
    };
}