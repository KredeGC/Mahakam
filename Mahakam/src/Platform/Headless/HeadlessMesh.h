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

		inline virtual const Bounds& GetBounds() const override { return m_Bounds; }

		inline virtual uint32_t GetVertexCount() const override { return m_MeshData.GetVertexCount(); }

		virtual const MeshData& GetMeshData() const override { return m_MeshData; }

		inline virtual const void* GetVertices(int index) const override { return m_MeshData.GetVertices(index); }

		inline virtual const uint32_t* GetIndices() const override { return m_MeshData.GetIndices().data(); }
		inline virtual uint32_t GetIndexCount() const override { return m_MeshData.GetIndexCount(); }
	};
}