#pragma once

#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	struct MeshComponent
	{
	private:
		Ref<Mesh> mesh;

	public:
		MeshComponent() : mesh(nullptr) {}

		MeshComponent(const MeshComponent&) = default;

		MeshComponent(const Ref<Mesh>& mesh) : mesh(mesh) {}

		operator Ref<Mesh>& () { return mesh; }
		operator const Ref<Mesh>& () const { return mesh; }
	};
}