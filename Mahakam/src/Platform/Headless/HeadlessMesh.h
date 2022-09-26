#pragma once

#include "Mahakam/Math/Bounds.h"
#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	class HeadlessMesh;

	extern template class Asset<HeadlessMesh>;

	class HeadlessMesh : public SubMesh
	{
	private:
		uint8_t* vertices[BUFFER_ELEMENTS_SIZE]{ 0 };

		uint32_t* indices = 0;

		uint32_t vertexCount;
		uint32_t indexCount;

		Bounds bounds;

	public:
		HeadlessMesh(uint32_t vertexCount, uint32_t indexCount, const void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices);
		virtual ~HeadlessMesh() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void RecalculateBounds() override;
		virtual void RecalculateNormals() override;
		virtual void RecalculateTangents() override;

		virtual void SetVertices(int slot, const void* data) override;

		virtual const Bounds& GetBounds() const override { return bounds; }

		inline uint32_t GetVertexCount() const override { return vertexCount; }

		inline virtual bool HasVertices(int index) const override { return vertices[index]; }
		inline virtual const void* GetVertices(int index) const override { return vertices[index]; }

		inline const uint32_t* GetIndices() const override { return indices; }
		inline uint32_t GetIndexCount() const override { return indexCount; }
	};
}