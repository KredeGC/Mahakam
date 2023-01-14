#pragma once

#include "Mahakam/Core/Allocator.h"
#include "Mahakam/Core/Types.h"
#include "Mahakam/Math/Bounds.h"
#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	class OpenGLMesh : public SubMesh
	{
	private:
		uint32_t m_RendererID;

		TrivialArray<uint8_t, Allocator::BaseAllocator<uint8_t>> m_InterleavedVertices;
		TrivialArray<uint8_t, Allocator::BaseAllocator<uint8_t>> m_Vertices[BUFFER_ELEMENTS_SIZE];
		TrivialArray<uint32_t, Allocator::BaseAllocator<uint32_t>> m_Indices;

		uint32_t m_VertexCount;
		uint32_t m_IndexCount;

		Bounds m_Bounds;

		uint32_t m_VertexBufferID;
		uint32_t m_IndexBufferID;

	public:
		OpenGLMesh(uint32_t vertexCount, uint32_t indexCount, const void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices);
		virtual ~OpenGLMesh() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void RecalculateBounds() override;
		virtual void RecalculateNormals() override;
		virtual void RecalculateTangents() override;

		virtual void SetVertices(int slot, const void* data) override;

		virtual const Bounds& GetBounds() const override { return m_Bounds; }

		inline uint32_t GetVertexCount() const override { return m_VertexCount; }

		inline virtual bool HasVertices(int index) const override { return m_Vertices[index].data(); }
		inline virtual const void* GetVertices(int index) const override { return m_Vertices[index].data(); }

		inline const uint32_t* GetIndices() const override { return m_Indices.data(); }
		inline uint32_t GetIndexCount() const override { return m_IndexCount; }

	private:
		void Init();

		void InterleaveBuffers();
	};
}