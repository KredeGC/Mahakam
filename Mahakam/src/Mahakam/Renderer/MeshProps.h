#pragma once

#include "Mahakam/Asset/Asset.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_float4x4.hpp>

#include <string>
#include <vector>

namespace Mahakam
{
	class Material;

	enum class MeshPrimitive
	{
		Model,
		Plane,
		Cube,
		CubeSphere,
		UVSphere
	};

	struct MeshNode
	{
		std::string Name; // Node name
		int ID; // Node ID
		int ParentID; // Parent node ID
		int Mesh; // Index of the submesh
		glm::mat4 Offset; // Offset. In order of priority: invMatrix, matrix, TRS
	};

	struct MeshProps
	{
		std::vector<Asset<Material>> Materials;
		MeshPrimitive Primitive = MeshPrimitive::Model;
		bool IncludeNodes = true; // TODO: Use flags instead of bools?
		bool IncludeBones = true;
		bool Invert = false;
		int Rows = 10;
		int Columns = 10;
	};
}