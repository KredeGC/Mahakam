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

		MeshComponent(Asset<Mesh> skinnedMesh) : m_SkinnedMesh(std::move(skinnedMesh)) {}

		MeshComponent(Asset<Mesh> skinnedMesh, Asset<Material> material) : m_SkinnedMesh(std::move(skinnedMesh))
		{
			m_SkinnedMesh->Props.Materials.clear();
			m_SkinnedMesh->Props.Materials.push_back(std::move(material));
		}

		MeshComponent(Ref<SubMesh> mesh, Asset<Material> material)
			: m_SkinnedMesh(CreateAsset<Mesh>(std::move(mesh), std::move(material))) {}

		inline void SetMesh(Asset<Mesh> skinnedMesh) { m_SkinnedMesh = std::move(skinnedMesh); }
		inline Asset<Mesh> GetMesh() const { return m_SkinnedMesh; }
		inline bool HasMesh() const { return bool(m_SkinnedMesh); }

		inline size_t GetSubMeshCount() const { return m_SkinnedMesh->Meshes.size(); }

		inline Ref<SubMesh> GetSubMesh() const { return m_SkinnedMesh->Meshes.at(0); }
		inline const std::vector<Ref<SubMesh>>& GetSubMeshes() const { return m_SkinnedMesh->Meshes; }

		inline Asset<Material> GetMaterial() const { return m_SkinnedMesh->Props.Materials.at(0); }
		inline const std::vector<Asset<Material>>& GetMaterials() const { return m_SkinnedMesh->Props.Materials; }

		inline const std::vector<MeshNode>& GetNodeHierarchy() const { return m_SkinnedMesh->NodeHierarchy; }
		inline const TrivialVector<uint32_t>& GetSkins() const { return m_SkinnedMesh->Skins; }
		inline const UnorderedMap<uint32_t, uint32_t>& GetSubMeshMap() const { return m_SkinnedMesh->SubMeshMap; }
		inline const UnorderedMap<uint32_t, uint32_t>& GetBoneInfo() const { return m_SkinnedMesh->BoneMap; }
	};
}