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

		MeshData m_MeshData;

		Bounds m_Bounds;

		uint32_t m_VertexBufferID;
		uint32_t m_IndexBufferID;

	public:
		OpenGLMesh(MeshData&& mesh);
		virtual ~OpenGLMesh() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void RecalculateBounds() override;
		virtual void RecalculateNormals() override;
		virtual void RecalculateTangents() override;

		virtual void SetVertices(int slot, const void* data) override;

		inline virtual const Bounds& GetBounds() const override { return m_Bounds; }

		inline virtual uint32_t GetVertexCount() const override { return m_MeshData.GetVertexCount(); }

		virtual const MeshData& GetMeshData() const override { return m_MeshData; }

		inline virtual const void* GetVertices(int index) const override { return m_MeshData.GetVertices(index); }

		inline virtual const uint32_t* GetIndices() const override { return m_MeshData.GetIndices().data(); }
		inline virtual uint32_t GetIndexCount() const override { return m_MeshData.GetIndexCount(); }

	private:
		void Init();
	};
}