#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Meta.h"

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

namespace tinygltf
{
	class Model;
}

namespace Mahakam
{
	class Bounds;
	class Material;
	class Shader;
	class SubMesh;

	class Mesh
	{
	public:
		MeshPrimitive Primitive;
		std::vector<Ref<SubMesh>> Meshes; // List of sub meshes

		Mesh() = default;

		explicit Mesh(MeshPrimitive primitive) :
			Primitive(primitive) {}

		virtual MeshProps& GetProps() = 0;

		inline static Asset<Mesh> Copy(Asset<Mesh> other) { return CopyImpl(std::move(other)); }

	private:
		MH_DECLARE_FUNC(CopyImpl, Asset<Mesh>, Asset<Mesh> other);
	};

	class BoneMesh : public Mesh
	{
	public:
		BoneMeshProps Props;
		std::vector<MeshNode> NodeHierarchy; // Hierarchy of nodes. Parents are always before children
		TrivialVector<uint32_t> Skins; // List of node indices that are skin roots
		UnorderedMap<uint32_t, uint32_t> SubMeshMap; // Hierarchy index to SubMesh index
		UnorderedMap<uint32_t, uint32_t> BoneMap; // Hierarchy index to Joint ID

		BoneMesh(const BoneMeshProps& props) :
			Mesh(MeshPrimitive::Model),
			Props(props) {}

		inline virtual MeshProps& GetProps() override { return Props; }

		inline static Asset<BoneMesh> Create(const BoneMeshProps& props) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<BoneMesh>, const BoneMeshProps& props);
	};

	class PlaneMesh : public Mesh
	{
	public:
		PlaneMeshProps Props;

		PlaneMesh(Ref<SubMesh> submesh, const PlaneMeshProps& props) :
			Mesh(MeshPrimitive::Plane),
			Props(props)
		{
			Meshes.push_back(std::move(submesh));
		}

		inline virtual MeshProps& GetProps() override { return Props; }

		inline static Asset<PlaneMesh> Create(const PlaneMeshProps& props) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<PlaneMesh>, const PlaneMeshProps& props);
	};

	class CubeMesh : public Mesh
	{
	public:
		CubeMeshProps Props;

		CubeMesh(Ref<SubMesh> submesh, const CubeMeshProps& props) :
			Mesh(MeshPrimitive::Cube),
			Props(props)
		{
			Meshes.push_back(std::move(submesh));
		}

		inline virtual MeshProps& GetProps() override { return Props; }

		inline static Asset<CubeMesh> Create(const CubeMeshProps& props) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<CubeMesh>, const CubeMeshProps& props);
	};

	class CubeSphereMesh : public Mesh
	{
	public:
		CubeSphereMeshProps Props;

		CubeSphereMesh(Ref<SubMesh> submesh, const CubeSphereMeshProps& props) :
			Mesh(MeshPrimitive::CubeSphere),
			Props(props)
		{
			Meshes.push_back(std::move(submesh));
		}

		inline virtual MeshProps& GetProps() override { return Props; }

		inline static Asset<CubeSphereMesh> Create(const CubeSphereMeshProps& props) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<CubeSphereMesh>, const CubeSphereMeshProps& props);
	};

	class UVSphereMesh : public Mesh
	{
	public:
		UVSphereMeshProps Props;

		UVSphereMesh(Ref<SubMesh> submesh, const UVSphereMeshProps& props) :
			Mesh(MeshPrimitive::UVSphere),
			Props(props)
		{
			Meshes.push_back(std::move(submesh));
		}

		inline virtual MeshProps& GetProps() override { return Props; }

		inline static Asset<UVSphereMesh> Create(const UVSphereMeshProps& props) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<UVSphereMesh>, const UVSphereMeshProps& props);
	};

	enum class VertexType
	{
		Position,
		TexCoords,
		Normals,
		Tangents,
		Colors,
		BoneIDs,
		BoneWeights
	};

	template<typename... Ts>
	class RawMeshData
	{
	public:
		inline static constexpr size_t size = sizeof...(Ts);

		// Provide some type safety
		template<auto I>
		void SetVertices(const detail::get_nth_from_variadric<size_t(I), Ts...>* data)
		{
			m_Vertices[size_t(I)] = data;
		}

		const void** GetData() { return m_Vertices; }

	private:
		const void* m_Vertices[size]{ nullptr };
	};

	using MeshStruct = RawMeshData<
		glm::vec3,
		glm::vec2,
		glm::vec3,
		glm::vec4,
		glm::vec4,
		glm::ivec4,
		glm::vec4
	>;

	class SubMesh
	{
	public:
		inline static constexpr uint32_t BUFFER_ELEMENTS_SIZE = 7U;
		inline static constexpr ShaderDataType BUFFER_ELEMENTS[BUFFER_ELEMENTS_SIZE]
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