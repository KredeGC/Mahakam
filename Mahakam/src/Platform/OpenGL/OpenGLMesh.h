#pragma once

#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	class OpenGLMesh : public Mesh
	{
	private:
		uint32_t rendererID;

		uint8_t* interleavedVertices = 0;
		std::unordered_map<int, void*> vertices; // TODO: Find some way to use robin_hood

		uint32_t* indices = 0;

		uint32_t vertexCount;
		uint32_t indexCount;

		Bounds bounds;

		uint32_t vertexBufferID;
		uint32_t indexBufferID;

	public:
		OpenGLMesh(uint32_t vertexCount, uint32_t indexCount, void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices);
		virtual ~OpenGLMesh() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void RecalculateBounds() override;
		virtual void RecalculateNormals() override;
		virtual void RecalculateTangents() override;

		virtual void SetVertices(int slot, const void* data) override;

		virtual const Bounds& GetBounds() const override { return bounds; }

		inline uint32_t GetVertexCount() const override { return vertexCount; }

		inline virtual bool HasVertices(int index) const override { return vertices.find(index) != vertices.end(); }
		inline virtual const void* GetVertices(int index) const override { auto it = vertices.find(index); return it != vertices.end() ? it->second : nullptr; }

		inline const uint32_t* GetIndices() const override { return indices; }
		inline uint32_t GetIndexCount() const override { return indexCount; }

	private:
		void InterleaveBuffers();
	};
}