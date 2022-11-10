#pragma once

#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	struct MeshComponent
	{
	private:
		Asset<Mesh> m_SkinnedMesh;

	public:
		MeshComponent() = default;

		MeshComponent(const MeshComponent&) = default;

		MeshComponent(Asset<Mesh> skinnedMesh) : m_SkinnedMesh(skinnedMesh) {}

		MeshComponent(Asset<Mesh> skinnedMesh, Asset<Material> material) : m_SkinnedMesh(skinnedMesh)
		{
			m_SkinnedMesh->Props.Materials.clear();
			m_SkinnedMesh->Props.Materials.push_back(material);
		}

		MeshComponent(Ref<SubMesh> mesh, Asset<Material> material)
			: m_SkinnedMesh(CreateAsset<Mesh>(mesh, material)) {}

		inline void SetMesh(Asset<Mesh> skinnedMesh) { m_SkinnedMesh = skinnedMesh; }
		inline Asset<Mesh> GetMesh() const { return m_SkinnedMesh; }
		inline bool HasMesh() const { return bool(m_SkinnedMesh); }

		inline size_t GetSubMeshCount() const { return m_SkinnedMesh->Meshes.size(); }

		inline Ref<SubMesh> GetSubMesh() { return m_SkinnedMesh->Meshes.at(0); }
		inline const std::vector<Ref<SubMesh>>& GetSubMeshes() { return m_SkinnedMesh->Meshes; }

		inline Asset<Material> GetMaterial() { return m_SkinnedMesh->Props.Materials.at(0); }
		inline const std::vector<Asset<Material>>& GetMaterials() { return m_SkinnedMesh->Props.Materials; }

		inline const std::vector<MeshNode>& GetNodeHierarchy() const { return m_SkinnedMesh->NodeHierarchy; }
		inline const UnorderedMap<std::string, int>& GetBoneInfo() const { return m_SkinnedMesh->BoneInfoMap; }
	};
}