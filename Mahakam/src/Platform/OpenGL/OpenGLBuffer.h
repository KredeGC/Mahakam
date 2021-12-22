#pragma once
#include "Mahakam/Renderer/Buffer.h"

namespace Mahakam
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	private:
		uint32_t rendererID;
		BufferLayout layout;

	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer() override;

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setLayout(const BufferLayout& layout) override;
		virtual const BufferLayout& getLayout() const override;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	private:
		uint32_t rendererID;
		uint32_t count;

	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer() override;

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual uint32_t getCount() const { return count; }
	};
}