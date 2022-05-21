#pragma once
#include "Mahakam/Renderer/Buffer.h"

namespace Mahakam
{
#pragma region OpenGLUniformBuffer
	class OpenGLUniformBuffer : public UniformBuffer
	{
	private:
		uint32_t m_RendererID;
		uint32_t m_Size;

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
		uint32_t m_RendererID;
		uint32_t m_Size;

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