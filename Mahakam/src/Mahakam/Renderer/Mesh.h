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
			return ShaderDataType::Float4;
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
			return 16;
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

		MeshElement() {}

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

		Ref<Material> material;

		void interleaveBuffers()
		{
			MH_PROFILE_FUNCTION();

			uint32_t stride = bufferLayout.getStride();
			uint32_t size = stride * vertexCount;

			interleavedVertices = new char[size];

			const std::vector<BufferElement>& elements = bufferLayout.getElements();

			uint32_t dstOffset = 0;
			for (uint32_t i = 0; i < vertexCount; i++)
			{
				uint32_t srcOffset = 0;
				for (auto& vert : vertices)
				{
					uint32_t size = elements[srcOffset++].size;
					char* src = vert.second.data + i * size;
					char* dst = interleavedVertices + dstOffset;

					memcpy(dst, src, size);
					dstOffset += size;
				}
			}
		}

		void initBuffers(bool interleave)
		{
			MH_PROFILE_FUNCTION();

			vertexArray = VertexArray::create();

			if (interleave)
			{
				Ref<VertexBuffer> vertexBuffer = VertexBuffer::create(interleavedVertices, bufferLayout.getStride() * vertexCount);
				vertexBuffer->setLayout(bufferLayout);
				vertexArray->addVertexBuffer(vertexBuffer);
			}
			else
			{
				const std::vector<BufferElement>& elements = bufferLayout.getElements();

				uint32_t offset = 0;
				for (auto& kv : vertices)
				{
					BufferLayout layout({ elements[offset] });

					Ref<VertexBuffer> vertexBuffer = VertexBuffer::create(kv.second.data, vertexCount * elements[offset].size);
					vertexBuffer->setLayout(layout);
					vertexArray->addVertexBuffer(vertexBuffer);
					offset++;
				}
			}

			Ref<IndexBuffer> indexBuffer = IndexBuffer::create(indices, indexCount);
			vertexArray->setIndexBuffer(indexBuffer);
		}

	public:
		Mesh(uint32_t vertexCount, const BufferLayout& layout, uint32_t indexCount)
			: vertexCount(vertexCount), bufferLayout(layout), indexCount(indexCount), indices(0), material(0) {}

		Mesh(uint32_t vertexCount, const BufferLayout& layout, const uint32_t* triangles, uint32_t indexCount)
			: vertexCount(vertexCount), bufferLayout(layout), indices(new uint32_t[indexCount]), indexCount(indexCount), material(0)
		{
			memcpy(indices, triangles, indexCount * sizeof(uint32_t));
		}

		Mesh(uint32_t vertexCount, const BufferLayout& layout, const uint32_t* triangles, uint32_t indexCount, const Ref<Material>& material)
			: vertexCount(vertexCount), bufferLayout(layout), indices(new uint32_t[indexCount]), indexCount(indexCount), material(material)
		{
			memcpy(indices, triangles, indexCount * sizeof(uint32_t));
		}

		~Mesh()
		{
			MH_PROFILE_FUNCTION();

			for (auto& kv : vertices)
				delete[] kv.second.data;

			delete[] interleavedVertices;
		}

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

		void setIndices(uint32_t* inds, unsigned int count)
		{
			MH_PROFILE_FUNCTION();

			// TODO: Update VAO if necessary
			indices = inds;
			indexCount = count;
		}

		void setMaterial(const Ref<Material>& mat)
		{
			material = mat;
		}

		template<typename T>
		inline const T& getVertices(int slot) const { return vertices[slot].data; }
		inline uint32_t getVertexCount() const { return vertexCount; }

		inline const uint32_t* getIndices() const { return indices; }
		inline uint32_t getIndexCount() const { return indexCount; }

		inline const Ref<Material>& getMaterial() const { return material; }
		inline const Ref<VertexArray>& getVertexArray() const { return vertexArray; }

		void bind()
		{
			MH_PROFILE_FUNCTION();

			//material->bind();
			vertexArray->bind();
		}

		const void* serialize(int& outLength)
		{
			outLength = sizeof(Mesh);

			void* data = new char[outLength];
			memcpy(data, this, outLength);

			return data;
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
			const uint32_t* indices, uint32_t indexCount, const Ref<Material>& material)
		{
			return std::make_shared<Mesh>(vertexCount, layout, indices, indexCount, material);
		}

		static Ref<Mesh> createCube(int tessellation);
		static Ref<Mesh> createPlane(int rows, int columns);
		static Ref<Mesh> createUVSphere(int rows, int columns);
		static Ref<Mesh> createCubeSphere(int tessellation, bool equirectangular = false);
	};
}