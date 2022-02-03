#pragma once

#include "Mahakam/Renderer/VertexArray.h"

namespace Mahakam
{
	class OpenGLVertexArray : public VertexArray
	{
	private:
		uint32_t rendererID;
		uint32_t vertexBufferIndex = 0;
		std::vector<Ref<VertexBuffer>> vertexBuffers;
		Ref<IndexBuffer> indexBuffer;

		uint32_t vertexCount;

	public:
		OpenGLVertexArray(uint32_t vertexCount);
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& buffer, bool interleave) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& buffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return vertexBuffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return indexBuffer; }
	};
}