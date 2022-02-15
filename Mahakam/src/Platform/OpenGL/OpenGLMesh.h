#pragma once

#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	class OpenGLMesh : public Mesh
	{
	private:
		uint32_t rendererID;

		char* interleavedVertices = 0;
		std::unordered_map<int, void*> vertices;

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

		template<typename T>
		inline const T& GetVertices(int slot) const { return vertices.at(slot); }
		inline uint32_t GetVertexCount() const override { return vertexCount; }

		inline const glm::vec3* GetPositions() const override { return (glm::vec3*)vertices.at(0); }
		inline const glm::vec2* GetTexcoords() const override { return (glm::vec2*)vertices.at(1); }
		inline const glm::vec3* GetNormals() const override { return (glm::vec3*)vertices.at(2); }
		inline const glm::vec3* GetTangents() const override { return (glm::vec3*)vertices.at(3); }
		inline const glm::vec4* GetColors() const override { return (glm::vec4*)vertices.at(4); }

		inline const uint32_t* GetIndices() const override { return indices; }
		inline uint32_t GetIndexCount() const override { return indexCount; }

	private:
		void InterleaveBuffers();
	};
}