#pragma once

#include "Mahakam/Core/Core.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Material.h"

#include "Assimp.h"

#include <map>

namespace Mahakam
{
	class Mesh;

	struct BoneInfo
	{
		int id;
		glm::mat4 offset;
	};

	struct SkinnedMesh
	{
		std::vector<Ref<Mesh>> meshes;
		std::vector<Ref<Material>> materials;
		std::map<std::string, BoneInfo> boneInfo;
		int boneCount = 0;
	};

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
		struct Vertex
		{
			char* data;
			uint32_t size;
			std::string name;
		};

	private:
		char* interleavedVertices = 0;
		uint32_t vertexCount = 0;

		std::unordered_map<int, Vertex> vertices;

		uint32_t* indices = 0;
		unsigned int indexCount = 0;

		bool interleave = true;

		BufferElement bufferElements[7]{
			{ ShaderDataType::Float3,	"i_Pos" },
			{ ShaderDataType::Float2,	"i_UV" },
			{ ShaderDataType::Float3,	"i_Normal" },
			{ ShaderDataType::Float3,	"i_Tangent" },
			{ ShaderDataType::Float4,	"i_Color" },
			{ ShaderDataType::Int4,		"i_BoneIDs" },
			{ ShaderDataType::Float4,	"i_BoneWeights" }
		};

		BufferLayout bufferLayout;

		Ref<VertexArray> vertexArray;

	public:
		Mesh(uint32_t vertexCount, uint32_t indexCount);

		Mesh(uint32_t vertexCount, const uint32_t* triangles, uint32_t indexCount);

		Mesh(uint32_t vertexCount, const uint32_t* triangles, uint32_t indexCount, const std::initializer_list<void*>& verts);

		Mesh(const Mesh& mesh);

		~Mesh();

		void SetVertices(const std::string& name, int index, const char* verts)
		{
			MH_PROFILE_FUNCTION();

			uint32_t elementSize = bufferElements[index].size;
			uint32_t size = elementSize * vertexCount;

			auto& iter = vertices.find(index);
			if (iter == vertices.end())
			{
				// The buffer doesn't exist
				Vertex vertex
				{
					new char[size],
					size,
					name
				};

				memcpy(vertex.data, verts, size);

				vertices[index] = vertex;
			}
			else
			{
				// The buffer already exists
				memcpy(vertices[index].data, verts, size);

				if (interleave)
				{
					InterleaveBuffers();

					const Ref<VertexBuffer>& buffer = vertexArray->GetVertexBuffers()[0];

					uint32_t bufferSize = bufferLayout.GetStride() * vertexCount;

					buffer->SetData(interleavedVertices, bufferSize);
				}
				else
				{
					const Ref<VertexBuffer>& buffer = vertexArray->GetVertexBuffers()[0];

					buffer->SetData(verts, size);
				}
			}
		}

		void Init(bool interleave = false)
		{
			MH_PROFILE_FUNCTION();

			this->interleave = interleave;

			InterleaveBuffers();
			InitBuffers();
		}

		// TODO: Update the actual buffers
		void SetIndices(uint32_t* inds, unsigned int count)
		{
			MH_PROFILE_FUNCTION();

			// TODO: Update VAO if necessary
			indices = inds;
			indexCount = count;
			// vertexArray::getIndexBuffer()->setData(...);
		}

		template<typename T>
		inline const T& GetVertices(int slot) const { return vertices[slot].data; }
		inline uint32_t GetVertexCount() const { return vertexCount; }

		inline const glm::vec3& GetPositions() const { return *(glm::vec3*)vertices.at(0).data; }
		inline const glm::vec2& GetTexcoords() const { return *(glm::vec2*)vertices.at(1).data; }
		inline const glm::vec3& GetNormals() const { return *(glm::vec3*)vertices.at(2).data; }
		inline const glm::vec3& GetTangents() const { return *(glm::vec3*)vertices.at(3).data; }
		inline const glm::vec4& GetColors() const { return *(glm::vec4*)vertices.at(4).data; }

		inline const uint32_t* GetIndices() const { return indices; }
		inline uint32_t GetIndexCount() const { return indexCount; }

		inline const Ref<VertexArray>& GetVertexArray() const { return vertexArray; }

		void Bind()
		{
			vertexArray->Bind();
		}

		void Unbind()
		{
			vertexArray->Unbind();
		}

		static Ref<Mesh> Create(uint32_t vertexCount, uint32_t indexCount)
		{
			return CreateRef<Mesh>(vertexCount, indexCount);
		}

		static Ref<Mesh> Create(uint32_t vertexCount, const uint32_t* indices, uint32_t indexCount)
		{
			return CreateRef<Mesh>(vertexCount, indices, indexCount);
		}

		static Ref<Mesh> Create(uint32_t vertexCount,
			const uint32_t* indices, uint32_t indexCount, const std::initializer_list<void*>& verts)
		{
			return CreateRef<Mesh>(vertexCount, indices, indexCount, verts);
		}

		static SkinnedMesh LoadModel(const std::string& filepath, const SkinnedMeshProps& props = SkinnedMeshProps());
		static Ref<Mesh> CreateCube(int tessellation, bool reverse = false);
		static Ref<Mesh> CreatePlane(int rows, int columns);
		static Ref<Mesh> CreateUVSphere(int rows, int columns);
		static Ref<Mesh> CreateCubeSphere(int tessellation, bool reverse = false, bool equirectangular = false);

	private:
		void InterleaveBuffers();

		void InitBuffers();

		static Ref<Mesh> ProcessMesh(SkinnedMesh& skinnedMesh, aiMesh* mesh, const aiScene* scene);

		static void ProcessNode(SkinnedMesh& skinnedMesh, aiNode* node, const aiScene* scene);
	};
}