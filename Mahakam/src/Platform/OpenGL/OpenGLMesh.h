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

		MeshData m_MeshData;

		uint32_t m_VertexCount;
		uint32_t m_IndexCount;

		Bounds m_Bounds;

		uint32_t m_VertexBufferID;
		uint32_t m_IndexBufferID;

	public:
		OpenGLMesh(uint32_t vertexCount, uint32_t indexCount, MeshData&& mesh, const uint32_t* indices);
		virtual ~OpenGLMesh() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void RecalculateBounds() override;
		virtual void RecalculateNormals() override;
		virtual void RecalculateTangents() override;

		virtual void SetVertices(int slot, const void* data) override;

		virtual const Bounds& GetBounds() const override { return m_Bounds; }

		inline uint32_t GetVertexCount() const override { return m_VertexCount; }

		inline virtual bool HasVertices(int index) const override { return m_MeshData.GetOffsets().find(index) != m_MeshData.GetOffsets().end(); }
		inline virtual const void* GetVertices(int index) const override { return m_MeshData.GetVertexData().data() + m_MeshData.GetOffsets().at(index).first; }

		inline const uint32_t* GetIndices() const override { return m_MeshData.GetIndices().data(); }
		inline uint32_t GetIndexCount() const override { return m_IndexCount; }

	private:
		void Init();
	};
}