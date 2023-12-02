#pragma once

#include "Mahakam/Math/Bounds.h"
#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	class HeadlessMesh : public SubMesh
	{
	private:
		MeshData m_MeshData;

		Bounds m_Bounds;

	public:
		HeadlessMesh(MeshData&& mesh);
		virtual ~HeadlessMesh() override = default;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void RecalculateBounds() override;
		virtual void RecalculateNormals() override;
		virtual void RecalculateTangents() override;

		virtual void SetVertices(int slot, const void* data) override;

		virtual const Bounds& GetBounds() const override { return m_Bounds; }

		inline uint32_t GetVertexCount() const override { return m_MeshData.GetVertexCount(); }

		inline virtual bool HasVertices(int index) const override { return m_MeshData.GetOffsets().find(index) != m_MeshData.GetOffsets().end(); }
		inline virtual const void* GetVertices(int index) const override { return m_MeshData.GetVertexData().data() + m_MeshData.GetOffsets().at(index).first; }

		inline const uint32_t* GetIndices() const override { return m_MeshData.GetIndices().data(); }
		inline uint32_t GetIndexCount() const override { return m_MeshData.GetIndexCount(); }
	};
}