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

		MeshComponent(const SkinnedMesh& skinnedMesh, Ref<Material> material) : skinnedMesh(skinnedMesh)
		{
			this->skinnedMesh.materials.clear();
			for (auto& kv : this->skinnedMesh.meshes)
				this->skinnedMesh.materials.push_back(material);
		}

		MeshComponent(Ref<Mesh> mesh)
		{
			this->skinnedMesh.meshes.push_back(mesh);
		}

		MeshComponent(Ref<Mesh> mesh, Ref<Material> material)
		{
			this->skinnedMesh.meshes.push_back(mesh);
			this->skinnedMesh.materials.push_back(material);
		}

		inline size_t getMeshCount() const { return skinnedMesh.meshes.size(); }

		inline Ref<Mesh> getMesh() { return skinnedMesh.meshes[0]; }
		inline const std::vector<Ref<Mesh>>& getMeshes() { return skinnedMesh.meshes; }

		inline Ref<Material> getMaterial() { return skinnedMesh.materials[0]; }
		inline const std::vector<Ref<Material>> getMaterials()& { return skinnedMesh.materials; }
	};
}