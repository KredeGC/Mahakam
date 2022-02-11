#pragma once

#include "Mahakam/Core/Core.h"
#include "Material.h"

#include "Assimp.h"

#include <robin_hood.h>

namespace Mahakam
{
	struct SkinnedMesh;

	// TODO: Use this prop struct when loading a model
	struct SkinnedMeshProps
	{
		bool createMaterials = false;
		std::vector<Ref<Material>> overrideMaterials;
		std::vector<Ref<Shader>> overrideShaders;

		SkinnedMeshProps() = default;

		SkinnedMeshProps(std::initializer_list<Ref<Shader>> shaders) : createMaterials(true), overrideShaders(shaders) {}

		SkinnedMeshProps(std::initializer_list<Ref<Material>> materials) : createMaterials(false), overrideMaterials(materials) {}
	};

	class Mesh
	{
	public:
		struct InterleavedStruct
		{
			glm::vec3* positions = nullptr;
			glm::vec2* texcoords = nullptr;
			glm::vec3* normals = nullptr;
			glm::vec3* tangents = nullptr;
			glm::vec4* colors = nullptr;
			glm::ivec4* boneIDs = nullptr;
			glm::vec4* boneWeights = nullptr;

			operator void** () { return (void**)this; }
		};

		struct Bounds
		{
			glm::vec3 min;
			glm::vec3 max;

			glm::vec3 positions[8];

			Bounds() = default;

			Bounds(const glm::vec3& min, const glm::vec3& max)
				: min(min), max(max)
			{
				positions[0] = min;
				positions[1] = { min.x, max.y, min.z };
				positions[2] = { min.x, min.y, max.z };
				positions[3] = { min.x, max.y, max.z };

				positions[4] = { max.x, min.y, min.z };
				positions[5] = { max.x, max.y, min.z };
				positions[6] = { max.x, min.y, max.z };
				positions[7] = max;
			}
		};

		static constexpr uint32_t BUFFER_ELEMENTS_SIZE = 7U;
		static constexpr ShaderDataType BUFFER_ELEMENTS[BUFFER_ELEMENTS_SIZE]{
			ShaderDataType::Float3, // Pos
			ShaderDataType::Float2, // UV
			ShaderDataType::Float3, // Normal
			ShaderDataType::Float3, // Tangent
			ShaderDataType::Float4, // Color
			ShaderDataType::Int4,   // BoneIDs
			ShaderDataType::Float4  // BoneWeights
		};

	public:
		virtual ~Mesh() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void RecalculateBounds() = 0;
		virtual void RecalculateNormals() = 0;
		virtual void RecalculateTangents() = 0;

		virtual void SetVertices(int slot, const void* data) = 0;

		virtual const Bounds& GetBounds() const = 0;

		virtual uint32_t GetVertexCount() const = 0;

		virtual const glm::vec3* GetPositions() const = 0;
		virtual const glm::vec2* GetTexcoords() const = 0;
		virtual const glm::vec3* GetNormals() const = 0;
		virtual const glm::vec3* GetTangents() const = 0;
		virtual const glm::vec4* GetColors() const = 0;

		virtual const uint32_t* GetIndices() const = 0;
		virtual uint32_t GetIndexCount() const = 0;

		static Bounds CalculateBounds(const glm::vec3* positions, uint32_t vertexCount);
		static Bounds TransformBounds(const Bounds& bounds, const glm::mat4& transform);

		static Ref<Mesh> Create(uint32_t vertexCount, uint32_t indexCount, void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices);

		static SkinnedMesh LoadModel(const std::string& filepath, const SkinnedMeshProps& props = SkinnedMeshProps());
		static Ref<Mesh> CreateCube(int tessellation, bool reverse = false);
		static Ref<Mesh> CreatePlane(int rows, int columns);
		static Ref<Mesh> CreateUVSphere(int rows, int columns);
		static Ref<Mesh> CreateCubeSphere(int tessellation, bool reverse = false, bool equirectangular = false);

	private:
		static Ref<Mesh> ProcessMesh(SkinnedMesh& skinnedMesh, aiMesh* mesh, const aiScene* scene);

		static void ProcessNode(SkinnedMesh& skinnedMesh, aiNode* node, const aiScene* scene);
	};

	struct BoneInfo
	{
		int id;
		glm::mat4 offset;
	};

	struct SkinnedMesh
	{
		std::vector<Ref<Mesh>> meshes;
		std::vector<Ref<Material>> materials;
		robin_hood::unordered_map<std::string, BoneInfo> boneInfo;
		int boneCount = 0;

		Mesh::Bounds bounds;

		SkinnedMesh() = default;

		SkinnedMesh(const std::vector<Ref<Mesh>>& meshes, const std::vector<Ref<Material>>& materials, const robin_hood::unordered_map<std::string, BoneInfo>& boneInfo, int boneCount = 0)
			: meshes(meshes), materials(materials), boneInfo(boneInfo), boneCount(boneCount)
		{
			//RecalculateBounds();
		}

		SkinnedMesh(Ref<Mesh> mesh, Ref<Material> material)
		{
			meshes.push_back(mesh);
			materials.push_back(material);

			//RecalculateBounds();
		}

		//void RecalculateBounds()
		//{
		//	glm::vec3* positions = new glm::vec3[meshes.size() * 2];
		//	for (size_t i = 0; i < meshes.size(); i++)
		//	{
		//		auto& buffer = meshes[i]->GetBounds().positions;

		//		positions[i * 2] = buffer[0]; // Min
		//		positions[i * 2 + 1] = buffer[7]; // Max
		//	}

		//	bounds = Mesh::CalculateBounds(positions, (uint32_t)meshes.size() * 2);

		//	delete[] positions;
		//}
	};
}