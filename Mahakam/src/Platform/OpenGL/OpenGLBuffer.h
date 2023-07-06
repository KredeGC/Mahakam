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
		OpenGLStorageBuffer(uint32_t size);
		~OpenGLStorageBuffer();

		void Bind(int slot, int offset, int size) const;
		void Unbind(int slot) const;

		uint32_t GetSize() const;

		void SetData(const void* data, uint32_t offset, uint32_t size);
	};
#pragma endregion


#pragma region OpenGLUniformBuffer
	class OpenGLUniformBuffer
	{
	private:
		uint32_t m_RendererID;
		uint32_t m_Size;

	public:
		OpenGLUniformBuffer(uint32_t size);
		~OpenGLUniformBuffer();

		void Bind(int slot, int offset, int size) const;
		void Unbind(int slot) const;

		void SetData(const void* data, uint32_t offset, uint32_t size);
	};
#pragma endregion
}