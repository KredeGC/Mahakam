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

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void addVertexBuffer(const Ref<VertexBuffer>& buffer, bool interleave) override;
		virtual void setIndexBuffer(const Ref<IndexBuffer>& buffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const { return vertexBuffers; }
		virtual const Ref<IndexBuffer>& getIndexBuffer() const { return indexBuffer; }
	};
}