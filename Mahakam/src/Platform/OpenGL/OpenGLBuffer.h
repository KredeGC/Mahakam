#pragma once

#include <cstdint>

namespace Mahakam
{
#pragma region OpenGLStorageBuffer
	class OpenGLStorageBuffer
	{
	private:
		uint32_t m_RendererID;
		uint32_t m_Size;

	public:
		OpenGLStorageBuffer() noexcept;
		OpenGLStorageBuffer(uint32_t size) noexcept;
		OpenGLStorageBuffer(const OpenGLStorageBuffer&) = delete;
		OpenGLStorageBuffer(OpenGLStorageBuffer&&) noexcept;
		~OpenGLStorageBuffer();

		OpenGLStorageBuffer& operator=(const OpenGLStorageBuffer&) = delete;
		OpenGLStorageBuffer& operator=(OpenGLStorageBuffer&&) noexcept;

		void Bind(int slot, int offset, int size) const noexcept;
		void Unbind(int slot) const noexcept;

		uint32_t GetSize() const noexcept;

		void SetData(const void* data, uint32_t offset, uint32_t size) noexcept;

	private:
		void release() noexcept;
	};
#pragma endregion


#pragma region OpenGLUniformBuffer
	class OpenGLUniformBuffer
	{
	private:
		uint32_t m_RendererID;
		uint32_t m_Size;

	public:
		OpenGLUniformBuffer() noexcept;
		OpenGLUniformBuffer(uint32_t size) noexcept;
		OpenGLUniformBuffer(const OpenGLUniformBuffer&) = delete;
		OpenGLUniformBuffer(OpenGLUniformBuffer&&) noexcept;
		~OpenGLUniformBuffer();

		OpenGLUniformBuffer& operator=(const OpenGLUniformBuffer&) = delete;
		OpenGLUniformBuffer& operator=(OpenGLUniformBuffer&&) noexcept;

		void Bind(int slot, int offset, int size) const noexcept;
		void Unbind(int slot) const noexcept;

		void SetData(const void* data, uint32_t offset, uint32_t size) noexcept;

	private:
		void release() noexcept;
	};
#pragma endregion
}