#pragma once

#include "Mahakam/Core/Core.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Material.h"

namespace Mahakam
{
	enum class ShaderSemantic
	{
		Position,
		Normal,
		Tangent,
		Color,
		TexCoord0 // Texcoord 0-9?
	};

	static ShaderDataType ShaderSemanticDataType(ShaderSemantic semantic)
	{
		switch (semantic)
		{
		case ShaderSemantic::Position:
			return ShaderDataType::Float3;
		case ShaderSemantic::Normal:
			return ShaderDataType::Float3;
		case ShaderSemantic::Tangent:
			return ShaderDataType::Float3;
		case ShaderSemantic::Color:
			return ShaderDataType::Float4;
		case ShaderSemantic::TexCoord0:
			return ShaderDataType::Float2;
		}

		MH_CORE_ASSERT(false, "Unknown shader semantic!");
		return ShaderDataType::None;
	}

	static uint32_t ShaderSemanticSize(ShaderSemantic semantic)
	{
		switch (semantic)
		{
		case ShaderSemantic::Position:
			return 12;
		case ShaderSemantic::Normal:
			return 12;
		case ShaderSemantic::Tangent:
			return 12;
		case ShaderSemantic::Color:
			return 16;
		case ShaderSemantic::TexCoord0:
			return 8;
		}

		MH_CORE_ASSERT(false, "Unknown shader semantic!");
		return 0;
	}

	struct MeshElement
	{
		ShaderSemantic semantic;
		std::string name;

		MeshElement() = default;

		MeshElement(ShaderSemantic semantic, const std::string& name)
			: semantic(semantic), name(name) {}
	};

	class MeshLayout
	{
	private:
		std::vector<MeshElement> elements;
		BufferLayout bufferLayout;

	public:
		MeshLayout(const std::initializer_list<MeshElement>& elements) : elements(elements)
		{
			MH_PROFILE_FUNCTION();

			std::vector<BufferElement> bufferElements;
			for (auto& kv : elements)
			{
				BufferElement el
				{
					ShaderSemanticDataType(kv.semantic),
					kv.name
				};

				bufferElements.push_back(el);
			}

			bufferLayout = bufferElements;
		}

		inline const BufferLayout& getBufferLayout() const { return bufferLayout; }
		inline const std::vector<MeshElement>& getElements() const { return elements; }

		std::vector<MeshElement>::iterator begin() { return elements.begin(); }
		std::vector<MeshElement>::iterator end() { return elements.end(); }
		std::vector<MeshElement>::const_iterator begin() const { return elements.begin(); }
		std::vector<MeshElement>::const_iterator end() const { return elements.end(); }
	};


	class Mesh
	{
	private:
		struct Vertex
		{
			char* data;
			std::string name;
		};

		bool interleaved = true;

		char* interleavedVertices = 0;
		uint32_t vertexCount = 0;
		std::unordered_map<int, Vertex> vertices;

		uint32_t* indices = 0;
		unsigned int indexCount = 0;

		BufferLayout bufferLayout;

		Ref<VertexArray> vertexArray;

		void interleaveBuffers();

		void initBuffers(bool interleave);

	public:
		Mesh(uint32_t vertexCount, const BufferLayout& layout, uint32_t indexCount);

		Mesh(uint32_t vertexCount, const BufferLayout& layout, const uint32_t* triangles, uint32_t indexCount);

		Mesh(uint32_t vertexCount, const BufferLayout& layout, const uint32_t* triangles, uint32_t indexCount, const std::initializer_list<void*>& verts);

		Mesh(const Mesh& mesh);

		~Mesh();

		// TODO: Merge with setVertices automatically call init()
		inline void addVertices(const std::string& name, const void* verts)
		{
			MH_PROFILE_FUNCTION();

			int index = (int)vertices.size();

			unsigned int stride = bufferLayout.getElement(index).size;
			uint32_t size = stride * vertexCount;

			Vertex vertex
			{
				new char[size],
				name
			};

			memcpy(vertex.data, verts, size);

			vertices[index] = vertex;
		}

		void setVertices(const std::string& name, int index, const char* verts)
		{
			MH_PROFILE_FUNCTION();

			unsigned int stride = bufferLayout.getElement(index).size;
			uint32_t elementSize = stride * vertexCount;

			memcpy(vertices[index].data, verts, elementSize);

			if (interleaved)
			{
				interleaveBuffers();

				const Ref<VertexBuffer>& buffer = vertexArray->getVertexBuffers()[0];

				uint32_t bufferSize = bufferLayout.getStride() * vertexCount;

				buffer->setData(interleavedVertices, bufferSize);
			}
			else
			{
				auto& buffers = vertexArray->getVertexBuffers();

				buffers[index]->setData(verts, elementSize);
			}
		}

		void init(bool interleave = true)
		{
			MH_PROFILE_FUNCTION();

			interleaved = interleave;

			if (interleave)
				interleaveBuffers();

			initBuffers(interleave);
		}

		// TODO: Update the actual buffers
		void setIndices(uint32_t* inds, unsigned int count)
		{
			MH_PROFILE_FUNCTION();

			// TODO: Update VAO if necessary
			indices = inds;
			indexCount = count;
		}

		template<typename T>
		inline const T& getVertices(int slot) const { return vertices[slot].data; }
		inline uint32_t getVertexCount() const { return vertexCount; }

		inline const uint32_t* getIndices() const { return indices; }
		inline uint32_t getIndexCount() const { return indexCount; }

		inline const Ref<VertexArray>& getVertexArray() const { return vertexArray; }

		void bind()
		{
			MH_PROFILE_FUNCTION();

			vertexArray->bind();
		}

		void unbind()
		{
			MH_PROFILE_FUNCTION();

			vertexArray->unbind();
		}

		static Ref<Mesh> create(uint32_t vertexCount, const BufferLayout& layout, uint32_t indexCount)
		{
			return std::make_shared<Mesh>(vertexCount, layout, indexCount);
		}

		static Ref<Mesh> create(uint32_t vertexCount, const BufferLayout& layout, const uint32_t* indices, uint32_t indexCount)
		{
			return std::make_shared<Mesh>(vertexCount, layout, indices, indexCount);
		}

		static Ref<Mesh> create(uint32_t vertexCount, const BufferLayout& layout,
			const uint32_t* indices, uint32_t indexCount, const std::initializer_list<void*>& verts)
		{
			return std::make_shared<Mesh>(vertexCount, layout, indices, indexCount, verts);
		}

		static Ref<Mesh> createScreenQuad();
		static Ref<Mesh> createCube(int tessellation, bool reverse = false);
		static Ref<Mesh> createPlane(int rows, int columns);
		static Ref<Mesh> createUVSphere(int rows, int columns);
		static Ref<Mesh> createCubeSphere(int tessellation, bool equirectangular = false);
	};
}