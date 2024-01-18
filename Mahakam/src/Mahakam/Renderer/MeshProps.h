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

	struct BoneMeshProps
	{
		MeshProps Base;
		std::filesystem::path Filepath;
		bool IncludeNodes = true; // TODO: Use flags instead of bools?
		bool IncludeBones = true;
	};

	struct CubeMeshProps
	{
		MeshProps Base;
		int Tessellation = 10;
		bool Invert = false;
	};

	struct CubeSphereMeshProps
	{
		MeshProps Base;
		int Tessellation = 10;
		bool Invert = false;
		bool Equirectangular = false;
	};

	struct PlaneMeshProps
	{
		MeshProps Base;
		int Rows = 10;
		int Columns = 10;
	};

	struct UVSphereMeshProps
	{
		MeshProps Base;
		int Rows = 10;
		int Columns = 10;
	};
}