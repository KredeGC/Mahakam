#pragma once

#include "Mahakam/Core/Core.h"

#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct aiNode;

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
        std::filesystem::path m_Filepath;

        std::vector<BoneTransform> m_BoneHierarchy;
        UnorderedMap<std::string, BoneInfo> m_BoneInfoMap;

        int m_AnimationIndex;
        std::string m_Name;
        float m_Duration;
        int m_TicksPerSecond;

    public:
        Animation() = default;

        Animation(const std::string& filepath, Mesh& skinnedMesh);

        inline const std::string& GetName() const { return m_Name; }

        inline const std::filesystem::path& GetFilepath() const { return m_Filepath; }

        inline int GetAnimationIndex() { return m_AnimationIndex; }

        inline int GetTicksPerSecond() { return m_TicksPerSecond; }

        inline float GetDuration() { return m_Duration; }

        inline const UnorderedMap<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

        inline const std::vector<BoneTransform>& GetBoneHierarchy() { return m_BoneHierarchy; }

        static Asset<Animation> Load(const std::string& filepath, Mesh& skinnedMesh);

    private:
        void ReadHierarchyData(int parentIndex, const aiNode* src);
    };
}