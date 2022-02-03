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

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const char* data, uint32_t size) override;

		virtual void SetLayout(const BufferLayout& layout) override;
		virtual const BufferLayout& GetLayout() const override;
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

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override { return count; }
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

		virtual void Bind(int slot, int offset, int size) const override;
		virtual void Unbind(int slot) const override;

		virtual void SetData(const void* data, uint32_t offset, uint32_t size) override;
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

		virtual void Bind(int slot, int offset, int size) const override;
		virtual void Unbind(int slot) const override;

		virtual uint32_t GetSize() const override;

		virtual void SetData(const void* data, uint32_t offset, uint32_t size) override;
	};
#pragma endregion
}