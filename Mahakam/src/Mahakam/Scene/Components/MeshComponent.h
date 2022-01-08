#pragma once

#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	struct MeshComponent
	{
	private:
		Ref<Mesh> mesh;
		Ref<Material> material;

	public:
		MeshComponent() : mesh(nullptr) {}

		MeshComponent(const MeshComponent&) = default;

		MeshComponent(const Ref<Mesh>& mesh) : mesh(mesh), material(0) {}

		MeshComponent(const Ref<Mesh>& mesh, const Ref<Material>& material) : mesh(mesh), material(material) {}

		Ref<Mesh>& getMesh() { return mesh; }
		const Ref<Mesh>& getMesh() const { return mesh; }

		Ref<Material>& getMaterial() { return material; }
		const Ref<Material>& getMaterial() const { return material; }
	};
}