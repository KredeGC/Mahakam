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
		float* vertices = 0;
		unsigned int vertexSize = 0;

		uint32_t* indices = 0;
		unsigned int indexCount = 0;

		BufferLayout bufferLayout;

		Ref<VertexArray> vertexArray;
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		Ref<Material> material;

	public:
		Mesh() : vertices(0), vertexSize(0), indices(0), indexCount(0), bufferLayout(), material(0) {}

		Mesh(float* vertices, unsigned int vertexSize, uint32_t* indices, unsigned int indexCount, const BufferLayout& layout)
			: vertices(vertices), vertexSize(vertexSize), indices(indices), indexCount(indexCount), bufferLayout(layout)
		{
			init();
		}

		Mesh(float* vertices, unsigned int vertexSize, uint32_t* indices, unsigned int indexCount,
			const BufferLayout& layout, const Ref<Material> mat)
			: vertices(vertices), vertexSize(vertexSize), indices(indices), indexCount(indexCount),
			bufferLayout(layout), material(mat)
		{
			init();
		}

		void setVertices(float* verts, unsigned int size, const BufferLayout& layout)
		{
			vertices = verts;
			vertexSize = size;
			bufferLayout = layout;
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

		inline const float* getVertices() const { return vertices; }
		inline uint32_t getVertexSize() const { return vertexSize; }

		inline const uint32_t* getIndices() const { return indices; }
		inline uint32_t getIndexCount() const { return indexCount; }

		inline const Ref<Material>& getMaterial() const { return material; }
		inline const Ref<VertexArray>& getVertexArray() const { return vertexArray; }

		void init()
		{
			vertexArray = VertexArray::create();

			vertexBuffer = VertexBuffer::create(vertices, vertexSize);
			vertexBuffer->setLayout(bufferLayout);
			vertexArray->addVertexBuffer(vertexBuffer);

			indexBuffer = IndexBuffer::create(indices, indexCount);
			vertexArray->setIndexBuffer(indexBuffer);
		}

		void bind()
		{
			material->bind();
			vertexArray->bind();
		}

		static Ref<Mesh> create() { return std::make_shared<Mesh>(); }

		static Ref<Mesh> create(float* vertices, unsigned int vertexSize, uint32_t* indices, unsigned int indexCount, const BufferLayout& layout)
		{
			return std::make_shared<Mesh>(vertices, vertexSize, indices, indexCount, layout);
		}

		static Ref<Mesh> create(float* vertices, unsigned int vertexSize, uint32_t* indices, unsigned int indexCount,
			const BufferLayout& layout, const Ref<Material>& material)
		{
			return std::make_shared<Mesh>(vertices, vertexSize, indices, indexCount, layout, material);
		}
	};
}