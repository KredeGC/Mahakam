#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Asset/Asset.h"
#include "ShaderDataTypes.h"

#include "Assimp.h"

namespace Mahakam
{
	class Material;
	class Shader;
	class Mesh;

	struct BoneInfo
	{
		int id;
		glm::mat4 offset;
	};

	struct SkinnedMesh
	{
		std::vector<Asset<Mesh>> meshes;
		std::vector<Asset<Material>> materials;
		UnorderedMap<std::string, BoneInfo> boneInfo;
		int boneCount = 0;

		SkinnedMesh() = default;

		SkinnedMesh(const std::vector<Asset<Mesh>>& meshes, const std::vector<Asset<Material>>& materials, const UnorderedMap<std::string, BoneInfo>& boneInfo, int boneCount = 0)
			: meshes(meshes), materials(materials), boneInfo(boneInfo), boneCount(boneCount)
		{ }

		SkinnedMesh(Asset<Mesh> mesh, Asset<Material> material)
		{
			meshes.push_back(mesh);
			materials.push_back(material);
		}
	};

	// TODO: Use this prop struct when loading a model
	struct SkinnedMeshProps
	{
		bool CreateMaterials = false;
		std::vector<Asset<Material>> OverrideMaterials;
		std::vector<Asset<Shader>> OverrideShaders;

		SkinnedMeshProps() = default;

		SkinnedMeshProps(std::initializer_list<Asset<Shader>> shaders) : CreateMaterials(true), OverrideShaders(shaders) {}

		SkinnedMeshProps(std::initializer_list<Asset<Material>> materials) : CreateMaterials(false), OverrideMaterials(materials) {}
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
		static constexpr ShaderDataType BUFFER_ELEMENTS[BUFFER_ELEMENTS_SIZE]
		{
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

		virtual bool HasVertices(int index) const = 0;
		virtual const void* GetVertices(int index) const = 0;

		const glm::vec3* GetPositions() const { return (glm::vec3*)GetVertices(0); }
		const glm::vec2* GetTexcoords() const { return (glm::vec2*)GetVertices(1); }
		const glm::vec3* GetNormals() const { return (glm::vec3*)GetVertices(2); }
		const glm::vec3* GetTangents() const { return (glm::vec3*)GetVertices(3); }
		const glm::vec4* GetColors() const { return (glm::vec4*)GetVertices(4); }
		const glm::ivec4* GetBoneIDs() const { return (glm::ivec4*)GetVertices(5); }
		const glm::vec4* GetBoneWeights() const { return (glm::vec4*)GetVertices(6); }

		virtual const uint32_t* GetIndices() const = 0;
		virtual uint32_t GetIndexCount() const = 0;

		static Bounds CalculateBounds(const glm::vec3* positions, uint32_t vertexCount);
		static Bounds TransformBounds(const Bounds& bounds, const glm::mat4& transform);

		inline static Asset<Mesh> Create(uint32_t vertexCount, uint32_t indexCount, void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices) { return CreateImpl(vertexCount, indexCount, verts, indices); }
		inline static SkinnedMesh LoadModel(const std::string& filepath, const SkinnedMeshProps& props = SkinnedMeshProps()) { return LoadModelImpl(filepath, props); }

		static Asset<Mesh> CreateCube(int tessellation, bool reverse = false);
		static Asset<Mesh> CreatePlane(int rows, int columns);
		static Asset<Mesh> CreateUVSphere(int rows, int columns);
		static Asset<Mesh> CreateCubeSphere(int tessellation, bool reverse = false, bool equirectangular = false);

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<Mesh>, uint32_t vertexCount, uint32_t indexCount, void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices);
		MH_DECLARE_FUNC(LoadModelImpl, SkinnedMesh, const std::string& filepath, const SkinnedMeshProps& props);

		static Asset<Mesh> ProcessMesh(SkinnedMesh& skinnedMesh, aiMesh* mesh, const aiScene* scene);

		static void ProcessNode(SkinnedMesh& skinnedMesh, aiNode* node, const aiScene* scene);
	};
}