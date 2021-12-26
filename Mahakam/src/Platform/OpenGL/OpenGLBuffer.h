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
		OpenGLVertexBuffer(const char* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer() override;

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setData(const char* data, uint32_t size) override;

		virtual void setLayout(const BufferLayout& layout) override;
		virtual const BufferLayout& getLayout() const override;
	};


#pragma region OpenGLIndexBuffer
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
#pragma endregion


#pragma region OpenGLUniformBuffer
	class OpenGLUniformBuffer : public UniformBuffer
	{
	private:
		uint32_t rendererID;
		uint32_t size;

	public:
		OpenGLUniformBuffer(uint32_t size);
		virtual ~OpenGLUniformBuffer() override;

		virtual void bind(int slot, int offset, int size) const override;
		virtual void unbind(int slot) const override;

		virtual void setData(const void* data, uint32_t offset, uint32_t size) override;
	};
#pragma endregion


#pragma region OpenGLStorageBuffer
	class OpenGLStorageBuffer : public StorageBuffer
	{
	private:
		uint32_t rendererID;
		uint32_t size;

	public:
		OpenGLStorageBuffer(uint32_t size);
		virtual ~OpenGLStorageBuffer() override;

		virtual void bind(int slot, int offset, int size) const override;
		virtual void unbind(int slot) const override;

		virtual void setData(const void* data, uint32_t offset, uint32_t size) override;
	};
#pragma endregion
}