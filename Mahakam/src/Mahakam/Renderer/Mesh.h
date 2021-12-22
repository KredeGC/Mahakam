#pragma once

#include "Mahakam/Core/Core.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Material.h"

namespace Mahakam
{
	class Mesh
	{
	private:
		struct Vertex
		{
			char* data;
			std::string name;
		};

		char* interleavedVertices = 0;
		uint32_t vertexCount = 0;
		std::unordered_map<int, Vertex> vertices;

		uint32_t* indices = 0;
		unsigned int indexCount = 0;

		BufferLayout bufferLayout;

		Ref<VertexArray> vertexArray;

		Ref<Material> material;

		void initBuffers(bool interleave)
		{
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
		Mesh() : vertices(0), vertexCount(0), indices(0), indexCount(0), bufferLayout(), material(0) {}

		Mesh(const char* vertices, unsigned int vertexCount,
			uint32_t* indices, unsigned int indexCount, const BufferLayout& layout)
			: interleavedVertices(new char[vertexCount * layout.getStride()]), vertexCount(vertexCount),
			indices(indices), indexCount(indexCount), bufferLayout(layout)
		{
			memcpy(interleavedVertices, vertices, vertexCount * layout.getStride());

			init();
		}

		Mesh(const char* vertices, unsigned int vertexCount, uint32_t* indices,
			unsigned int indexCount, const BufferLayout& layout, const Ref<Material> mat)
			: interleavedVertices(new char[vertexCount * layout.getStride()]), vertexCount(vertexCount),
			indices(indices), indexCount(indexCount), bufferLayout(layout), material(mat)
		{
			memcpy(interleavedVertices, vertices, vertexCount * layout.getStride());

			init();
		}

		~Mesh()
		{
			for (auto& kv : vertices)
				delete kv.second.data;

			delete[] interleavedVertices;
		}

		void setLayout(const BufferLayout& layout)
		{
			bufferLayout = layout;
		}

		void setVertexCount(int count)
		{
			vertexCount = count;
		}

		inline void addVertices(const std::string& name, const void* verts)
		{
			setVertices(name, (int)vertices.size(), verts);
		}

		void setVertices(const std::string& name, int index, const void* verts)
		{
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

		void init(bool interleave = true)
		{
			if (interleave)
			{
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

			initBuffers(interleave);
		}

		void setIndices(uint32_t* inds, unsigned int count)
		{
			indices = inds;
			indexCount = count;
		}

		void setMaterial(const Ref<Material>& mat)
		{
			material = mat;
		}

		template<typename T>
		inline const T& getVertices(int slot) const { return vertices[slot].data; }
		inline uint32_t getVertexSize() const { return vertexCount; }

		inline const uint32_t* getIndices() const { return indices; }
		inline uint32_t getIndexCount() const { return indexCount; }

		inline const Ref<Material>& getMaterial() const { return material; }
		inline const Ref<VertexArray>& getVertexArray() const { return vertexArray; }

		void bind()
		{
			material->bind();
			vertexArray->bind();
		}

		static Ref<Mesh> create() { return std::make_shared<Mesh>(); }

		static Ref<Mesh> create(char* vertices, unsigned int vertexSize, uint32_t* indices, unsigned int indexCount, const BufferLayout& layout)
		{
			return std::make_shared<Mesh>(vertices, vertexSize, indices, indexCount, layout);
		}

		static Ref<Mesh> create(char* vertices, unsigned int vertexSize, uint32_t* indices, unsigned int indexCount,
			const BufferLayout& layout, const Ref<Material>& material)
		{
			return std::make_shared<Mesh>(vertices, vertexSize, indices, indexCount, layout, material);
		}

		static Ref<Mesh> createCube(unsigned int tessellation)
		{
			const int mods = 2 + 3; // UV + normals

			float vertices[24 * (3 + mods)]
			{
				-0.5f, 0.0f, -0.5f, 0.0f, 0.0f,
				 0.5f, 0.0f, -0.5f, 1.0f, 0.0f,
				 0.5f, 0.0f,  0.5f, 1.0f, 1.0f,
				-0.5f, 0.0f,  0.5f, 0.0f, 1.0f,
			};


		}
	};
}