#pragma once

#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	struct MeshComponent
	{
	private:
		SkinnedMesh skinnedMesh;

	public:
		MeshComponent() = default;

		MeshComponent(const MeshComponent&) = default;

		MeshComponent(const SkinnedMesh& skinnedMesh) : skinnedMesh(skinnedMesh) {}

		MeshComponent(const SkinnedMesh& skinnedMesh, Asset<Material> material) : skinnedMesh(skinnedMesh)
		{
			this->skinnedMesh.materials.clear();
			this->skinnedMesh.materials.push_back(material);
		}

		MeshComponent(Asset<Mesh> mesh, Asset<Material> material)
			: skinnedMesh(mesh, material) {}

		inline size_t GetMeshCount() const { return skinnedMesh.meshes.size(); }

		inline Asset<Mesh> GetMesh() { return skinnedMesh.meshes[0]; }
		inline const std::vector<Asset<Mesh>>& GetMeshes() { return skinnedMesh.meshes; }

		inline Asset<Material> GetMaterial() { return skinnedMesh.materials[0]; }
		inline const std::vector<Asset<Material>>& GetMaterials() { return skinnedMesh.materials; }
	};
}