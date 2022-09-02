#pragma once

#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	struct MeshComponent
	{
	private:
		Asset<SkinnedMesh> m_SkinnedMesh;

	public:
		MeshComponent() = default;

		MeshComponent(const MeshComponent&) = default;

		MeshComponent(Asset<SkinnedMesh> skinnedMesh) : m_SkinnedMesh(skinnedMesh) {}

		MeshComponent(Asset<SkinnedMesh> skinnedMesh, Asset<Material> material) : m_SkinnedMesh(skinnedMesh)
		{
			m_SkinnedMesh->materials.clear();
			m_SkinnedMesh->materials.push_back(material);
		}

		MeshComponent(Asset<Mesh> mesh, Asset<Material> material)
			: m_SkinnedMesh(CreateRef<SkinnedMesh>(mesh, material)) {}

		inline void SetMesh(Asset<SkinnedMesh> skinnedMesh) { m_SkinnedMesh = skinnedMesh; }
		inline Asset<SkinnedMesh> GetMesh() const { return m_SkinnedMesh; }
		inline bool HasMesh() const { return bool(m_SkinnedMesh); }

		inline size_t GetSubMeshCount() const { return m_SkinnedMesh->meshes.size(); }

		inline Asset<Mesh> GetSubMesh() { return m_SkinnedMesh->meshes.at(0); }
		inline const std::vector<Asset<Mesh>>& GetSubMeshes() { return m_SkinnedMesh->meshes; }

		inline Asset<Material> GetMaterial() { return m_SkinnedMesh->materials.at(0); }
		inline const std::vector<Asset<Material>>& GetMaterials() { return m_SkinnedMesh->materials; }

		inline const std::vector<BoneNode>& GetBoneHierarchy() const { return m_SkinnedMesh->BoneHierarchy; }
		inline const UnorderedMap<std::string, BoneInfo>& GetBoneInfo() const { return m_SkinnedMesh->boneInfo; }
	};
}