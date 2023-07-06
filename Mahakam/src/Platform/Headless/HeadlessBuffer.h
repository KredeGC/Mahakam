#pragma once

#include <cstdint>

namespace Mahakam
{
#pragma region HeadlessStorageBuffer
	class HeadlessStorageBuffer
	{
	private:
		uint32_t m_Size;
        uint8_t* m_Data = nullptr;

	public:
		HeadlessStorageBuffer(uint32_t size);
		~HeadlessStorageBuffer();

		void Bind(int slot, int offset, int size) const;
		void Unbind(int slot) const;

		uint32_t GetSize() const;

		void SetData(const void* data, uint32_t offset, uint32_t size);
	};
#pragma endregion


#pragma region HeadlessUniformBuffer
	class HeadlessUniformBuffer
	{
	private:
		uint32_t m_Size;
		uint8_t* m_Data = nullptr;

	public:
		HeadlessUniformBuffer(uint32_t size);
		~HeadlessUniformBuffer();

		void Bind(int slot, int offset, int size) const;
		void Unbind(int slot) const;

		void SetData(const void* data, uint32_t offset, uint32_t size);
	};
#pragma endregion
}