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
		MeshPrimitive Primitive; // TODO: Remove
		Ref<MeshProps> Props; // TODO: Unref this so that copying actually works
		std::vector<Ref<SubMesh>> Meshes; // List of sub meshes
		std::vector<MeshNode> NodeHierarchy; // Hierarchy of nodes. Parents are always before children
		TrivialVector<uint32_t> Skins; // List of node indices that are skin roots
		UnorderedMap<uint32_t, uint32_t> SubMeshMap; // Hierarchy index to SubMesh index
		UnorderedMap<uint32_t, uint32_t> BoneMap; // Hierarchy index to Joint ID

		Mesh() = default;

		explicit Mesh(MeshPrimitive primitive) :
			Primitive(primitive) {}

		explicit Mesh(MeshPrimitive primitive, const MeshProps& props) :
			Primitive(primitive),
			Props(CreateRef<MeshProps>(props)) {}

		explicit Mesh(const MeshProps& props) :
			Primitive(MeshPrimitive::Model),
			Props(CreateRef<MeshProps>(props)) {}

		MeshProps& GetProps() { return *Props; }

		inline static Asset<Mesh> Copy(Asset<Mesh> other) { return CopyImpl(std::move(other)); }

		inline static Asset<Mesh> Load(const std::filesystem::path& filepath, const MeshProps& props) { return LoadImpl(filepath, props); }

	private:
		MH_DECLARE_FUNC(CopyImpl, Asset<Mesh>, Asset<Mesh> other);

		MH_DECLARE_FUNC(LoadImpl, Asset<Mesh>, const std::filesystem::path& filepath, const MeshProps& props);
	};

	using Model = Mesh;

	using BoneMesh = Mesh;

	class PlaneMesh : public Mesh
	{
	public:
		PlaneMesh(Ref<SubMesh> submesh, const PlaneMeshProps& props) :
			Mesh(MeshPrimitive::Plane, props)
		{
			Meshes.push_back(std::move(submesh));
		}

		inline static Asset<PlaneMesh> Create(const PlaneMeshProps& props) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<PlaneMesh>, const PlaneMeshProps& props);
	};

	class CubeMesh : public Mesh
	{
	public:
		CubeMesh(Ref<SubMesh> submesh, const CubeMeshProps& props) :
			Mesh(MeshPrimitive::Cube, props)
		{
			Meshes.push_back(std::move(submesh));
		}

		inline static Asset<CubeMesh> Create(const CubeMeshProps& props) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<CubeMesh>, const CubeMeshProps& props);
	};

	class CubeSphereMesh : public Mesh
	{
	public:
		CubeSphereMesh(Ref<SubMesh> submesh, const CubeSphereMeshProps& props) :
			Mesh(MeshPrimitive::CubeSphere, props)
		{
			Meshes.push_back(std::move(submesh));
		}

		inline static Asset<CubeSphereMesh> Create(const CubeSphereMeshProps& props) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<CubeSphereMesh>, const CubeSphereMeshProps& props);
	};

	class UVSphereMesh : public Mesh
	{
	public:
		UVSphereMesh(Ref<SubMesh> submesh, const UVSphereMeshProps& props) :
			Mesh(MeshPrimitive::UVSphere, props)
		{
			Meshes.push_back(std::move(submesh));
		}

		inline static Asset<UVSphereMesh> Create(const UVSphereMeshProps& props) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<UVSphereMesh>, const UVSphereMeshProps& props);
	};

	enum class VertexType
	{
		Position = 0,
		TexCoords,
		Normals,
		Tangents,
		Colors,
		BoneIDs,
		BoneWeights
	};

	class MeshData
	{
	public:
		using Input = int;

		MeshData() :
			m_VertexCount(0),
			m_VertexData(Allocator::GetAllocator<uint8_t>()),
			m_Indices(Allocator::GetAllocator<uint32_t>()),
			m_Offsets(Allocator::GetAllocator<std::pair<const Input, std::pair<size_t, ShaderDataType>>>()) {}

		MeshData(uint32_t vertexCount) :
			m_VertexCount(vertexCount),
			m_VertexData(Allocator::GetAllocator<uint8_t>()),
			m_Indices(Allocator::GetAllocator<uint32_t>()),
			m_Offsets(Allocator::GetAllocator<std::pair<const Input, std::pair<size_t, ShaderDataType>>>()) {}

		MeshData(uint32_t vertexCount, const uint32_t* indices, uint32_t indexCount) :
			m_VertexCount(vertexCount),
			m_VertexData(Allocator::GetAllocator<uint8_t>()),
			m_Indices(indices, indices + indexCount, Allocator::GetAllocator<uint32_t>()),
			m_Offsets(Allocator::GetAllocator<std::pair<const Input, std::pair<size_t, ShaderDataType>>>()) {}

		template<typename T>
		MeshData(uint32_t vertexCount, T&& container) :
			m_VertexCount(vertexCount),
			m_VertexData(Allocator::GetAllocator<uint8_t>()),
			m_Indices(std::forward<T>(container)),
			m_Offsets(Allocator::GetAllocator<std::pair<const Input, std::pair<size_t, ShaderDataType>>>()) {}

		MeshData(const MeshData&) = delete;

		MeshData(MeshData&&) noexcept = default;

		void SetVertices(Input index, ShaderDataType dataType, const void* data, size_t size)
		{
			auto iter = m_Offsets.find(index);
			if (iter != m_Offsets.end())
			{
				uint8_t* begin = m_VertexData.begin() + iter->second.first;

				std::memcpy(begin, data, m_VertexCount * size);
			}
			else
			{
				m_Offsets[index] = std::make_pair(m_VertexData.size(), dataType);
				m_VertexData.push_back(reinterpret_cast<const uint8_t*>(data), reinterpret_cast<const uint8_t*>(data) + m_VertexCount * size);
			}
		}

		template<typename U, typename T>
		void SetVertices(U index, ShaderDataType dataType, const T* data)
		{
			SetVertices(Input(index), dataType, data, sizeof(T));
		}

		template<typename T>
		const void* GetVertices(T index) const
		{
			auto iter = m_Offsets.find(size_t(index));
			if (iter == m_Offsets.end())
				return nullptr;

			return m_VertexData.data() + iter->first;
		}

		template<typename T, typename U>
		const T* GetVertices(U index) const
		{
			return reinterpret_cast<const T*>(GetVertices(index));
		}

		void SetIndices(const uint32_t* data, uint32_t indexCount)
		{
			m_Indices.assign(data, data + indexCount);
		}

		template<typename T>
		void SetIndices(T&& container)
		{
			m_Indices = std::forward<T>(container);
		}

		uint32_t GetVertexCount() const { return m_VertexCount; }
		uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_Indices.size()); }

		const auto& GetVertexData() const { return m_VertexData; }
		const auto& GetOffsets() const { return m_Offsets; }
		const auto& GetIndices() const { return m_Indices; }

	private:
		uint32_t m_VertexCount;
		TrivialVector<uint8_t, Allocator::BaseAllocator<uint8_t>> m_VertexData;
		TrivialArray<uint32_t, Allocator::BaseAllocator<uint32_t>> m_Indices;
		UnorderedMap<Input, std::pair<size_t, ShaderDataType>, Allocator::BaseAllocator<std::pair<const Input, std::pair<size_t, ShaderDataType>>>> m_Offsets;
	};

	class SubMesh
	{
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

		virtual const MeshData& GetMeshData() const = 0;

		virtual const void* GetVertices(int index) const = 0;

		virtual const uint32_t* GetIndices() const = 0;
		virtual uint32_t GetIndexCount() const = 0;

		bool HasVertices(int index) { return GetVertices(index) == nullptr; }

		const glm::vec3* GetPositions() const	{ return reinterpret_cast<const glm::vec3*>(GetVertices(0)); }
		const glm::vec2* GetTexcoords() const	{ return reinterpret_cast<const glm::vec2*>(GetVertices(1)); }
		const glm::vec3* GetNormals() const		{ return reinterpret_cast<const glm::vec3*>(GetVertices(2)); }
		const glm::vec4* GetTangents() const	{ return reinterpret_cast<const glm::vec4*>(GetVertices(3)); }
		const glm::vec4* GetColors() const		{ return reinterpret_cast<const glm::vec4*>(GetVertices(4)); }
		const glm::ivec4* GetBoneIDs() const	{ return reinterpret_cast<const glm::ivec4*>(GetVertices(5)); }
		const glm::vec4* GetBoneWeights() const { return reinterpret_cast<const glm::vec4*>(GetVertices(6)); }

		inline static Ref<SubMesh> Create(MeshData&& mesh) { return CreateImpl(std::move(mesh)); }
		
		static Ref<SubMesh> CreateCube(int tessellation, bool reverse = false);
		static Ref<SubMesh> CreatePlane(int rows, int columns);
		static Ref<SubMesh> CreateUVSphere(int rows, int columns);
		static Ref<SubMesh> CreateCubeSphere(int tessellation, bool reverse = false, bool equirectangular = false);

	private:
		MH_DECLARE_FUNC(CreateImpl, Ref<SubMesh>, MeshData&& mesh);
	};
}