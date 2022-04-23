#pragma once

#include "Mahakam/Core/Core.h"

#include "Bone.h"
#include "Mesh.h"

#include <robin_hood.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>

namespace Mahakam
{
    struct BoneTransform
    {
        glm::mat4 transformation;
        std::string name;
        int parentIndex;
    };

    class Animation
    {
    private:
        std::string m_Name;
        float m_Duration;
        int m_TicksPerSecond;
        std::unordered_map<std::string, Bone> m_Bones;
        std::vector<BoneTransform> m_BoneHierarchy;
        robin_hood::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

    public:
        Animation() = default;

        Animation(const std::string& filepath, SkinnedMesh& skinnedMesh);

        Bone* FindBone(const std::string& name);

        inline const std::string& GetName() const { return m_Name; }

        inline int GetTicksPerSecond() { return m_TicksPerSecond; }

        inline float GetDuration() { return m_Duration; }

        inline const std::vector<BoneTransform>& GetBoneHierarchy() const { return m_BoneHierarchy; }

        inline const robin_hood::unordered_map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

        static Ref<Animation> Load(const std::string& filepath, SkinnedMesh& skinnedMesh);

    private:
        void ReadMissingBones(const aiAnimation* animation, robin_hood::unordered_map<std::string, BoneInfo>& boneInfoMap, int& boneCount);

        void ReadHierarchyData(int parentIndex, const aiNode* src);
    };
}