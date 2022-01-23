#pragma once
#include "Buffer.h"

#include <memory>

namespace Mahakam
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() {}

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void addVertexBuffer(const Ref<VertexBuffer>& buffer, bool interleave = true) = 0;
		virtual void setIndexBuffer(const Ref<IndexBuffer>& buffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& getIndexBuffer() const = 0;

		static Ref<VertexArray> create(uint32_t vertexCount);
	};
}