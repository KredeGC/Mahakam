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
		int ParentID; // Parent node ID or -1
		glm::mat4 Offset; // Offset. In order of priority: invMatrix, matrix, TRS
	};

	struct MeshProps
	{
		std::vector<Asset<Material>> Materials;
	};

	struct BoneMeshProps : public MeshProps
	{
		std::filesystem::path Filepath;
		bool IncludeNodes = true; // TODO: Use flags instead of bools?
		bool IncludeBones = true;
	};

	// Preferably this would only be needed in the editor
	// Once a mesh has been imported, these settings should not matter
	// They could even be made into their own Mesh-classes instead of deriving from MeshProps
	// Compared to BoneMeshProps, these props only specify how to generate the mesh, not how to use it afterwards
	struct PlaneMeshProps : public MeshProps
	{
		int Rows = 10;
		int Columns = 10;
	};

	struct CubeMeshProps : public MeshProps
	{
		int Tessellation = 10;
		bool Invert = false;
	};

	struct CubeSphereMeshProps : public MeshProps
	{
		int Tessellation = 10;
		bool Invert = false;
		bool Equirectangular = false;
	};

	struct UVSphereMeshProps : public MeshProps
	{
		int Rows = 10;
		int Columns = 10;
	};
}