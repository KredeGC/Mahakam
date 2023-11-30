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

		// TODO: Remove m_Vertices entirely and instead provide mapping from string to offset eg. "i_Pos" -> 42
		// This way we save memory and can still modify the mesh since we know the offsets
		// This will also require shaders to reflect inputs per stage as the data blob "could" in theory be in any order
		// To start I could use location mapping instead of string to avoid more shader reflection

		TrivialArray<uint8_t, Allocator::BaseAllocator<uint8_t>> m_InterleavedVertices;
		TrivialArray<uint32_t, Allocator::BaseAllocator<uint32_t>> m_Indices;

		UnorderedMap<int, size_t, Allocator::BaseAllocator<std::pair<const int, size_t>>> m_Offsets;

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

		inline virtual bool HasVertices(int index) const override { return m_Offsets.find(index) != m_Offsets.end(); }
		inline virtual const void* GetVertices(int index) const override { return m_InterleavedVertices.data() + m_Offsets.at(index); }

		inline const uint32_t* GetIndices() const override { return m_Indices.data(); }
		inline uint32_t GetIndexCount() const override { return m_IndexCount; }

	private:
		void Init(uint32_t vertexCount, const void** verts, const ShaderDataType* inputs, uint32_t inputCount);

		void InterleaveBuffers(uint32_t vertexCount, const void** verts, const ShaderDataType* inputs, uint32_t inputCount);
	};
}