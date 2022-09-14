#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

#include "MeshProps.h"
#include "ShaderDataTypes.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_int4.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace tinygltf
{
	class Model;
}

namespace Mahakam
{
	class Bounds;
	class Material;
	class Mesh;
	class Shader;
	class SubMesh;

	extern template class Asset<Material>;
	extern template class Asset<Mesh>;
	extern template class Asset<Shader>;
	extern template class Asset<SubMesh>;

	class Mesh
	{
	public:
		MeshProps Props;
		std::vector<Ref<SubMesh>> Meshes;
		std::vector<MeshNode> NodeHierarchy;
		UnorderedMap<std::string, int> BoneInfoMap; // name to Joint ID

		Mesh() = default;

		explicit Mesh(const MeshProps & props) :
			Props(props) {}

		Mesh(Ref<SubMesh> mesh, Asset<Material> material)
		{
			Meshes.push_back(mesh);
			Props.Materials.push_back(material);
		}

		Mesh(Ref<SubMesh> mesh, const MeshProps& props) :
			Props(props)
		{
			Meshes.push_back(mesh);
		}

		inline static Ref<Mesh> LoadMesh(const std::filesystem::path& filepath, const MeshProps& props = MeshProps()) { return LoadMeshImpl(filepath, props); }
		static Asset<Mesh> Copy(Asset<Mesh> copy) { return Asset<Mesh>(CreateRef<Mesh>(*copy.RefPtr())); }

	private:
		static void GLTFReadNodeHierarchy(const tinygltf::Model& model, UnorderedMap<int, size_t>& nodeIndex, int id, int parentID, Ref<Mesh> skinnedMesh);
		MH_DECLARE_FUNC(LoadMeshImpl, Ref<Mesh>, const std::filesystem::path& filepath, const MeshProps& props);
	};

	class SubMesh
	{
	public:
		struct InterleavedStruct
		{
			glm::vec3* positions = nullptr;
			glm::vec2* texcoords = nullptr;
			glm::vec3* normals = nullptr;
			glm::vec4* tangents = nullptr;
			glm::vec4* colors = nullptr;
			glm::ivec4* boneIDs = nullptr;
			glm::vec4* boneWeights = nullptr;

			operator void** () { return (void**)this; }
			operator const void** () const { return (const void**)this; }
		};

		static constexpr uint32_t BUFFER_ELEMENTS_SIZE = 7U;
		static constexpr ShaderDataType BUFFER_ELEMENTS[BUFFER_ELEMENTS_SIZE]
		{
			ShaderDataType::Float3, // Pos
			ShaderDataType::Float2, // UV
			ShaderDataType::Float3, // Normal
			ShaderDataType::Float4, // Tangent
			ShaderDataType::Float4, // Color
			ShaderDataType::Int4,   // BoneIDs
			ShaderDataType::Float4  // BoneWeights
		};

	public:
		virtual ~SubMesh() = default;

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

		const glm::vec3* GetPositions() const { return (const glm::vec3*)GetVertices(0); }
		const glm::vec2* GetTexcoords() const { return (const glm::vec2*)GetVertices(1); }
		const glm::vec3* GetNormals() const { return (const glm::vec3*)GetVertices(2); }
		const glm::vec4* GetTangents() const { return (const glm::vec4*)GetVertices(3); }
		const glm::vec4* GetColors() const { return (const glm::vec4*)GetVertices(4); }
		const glm::ivec4* GetBoneIDs() const { return (const glm::ivec4*)GetVertices(5); }
		const glm::vec4* GetBoneWeights() const { return (const glm::vec4*)GetVertices(6); }

		virtual const uint32_t* GetIndices() const = 0;
		virtual uint32_t GetIndexCount() const = 0;

		inline static Ref<SubMesh> Create(uint32_t vertexCount, uint32_t indexCount, const void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices) { return CreateImpl(vertexCount, indexCount, verts, indices); }
		
		static Ref<SubMesh> CreateCube(int tessellation, bool reverse = false);
		static Ref<SubMesh> CreatePlane(int rows, int columns);
		static Ref<SubMesh> CreateUVSphere(int rows, int columns);
		static Ref<SubMesh> CreateCubeSphere(int tessellation, bool reverse = false, bool equirectangular = false);

	private:
		MH_DECLARE_FUNC(CreateImpl, Ref<SubMesh>, uint32_t vertexCount, uint32_t indexCount, const void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices);
	};
}