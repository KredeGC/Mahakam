#pragma once

#include <cstdint>

namespace Mahakam
{
#pragma region HeadlessStorageBuffer
	class HeadlessStorageBuffer
	{
	private:
		uint32_t m_Size;

	public:
		HeadlessStorageBuffer() noexcept = default;
		HeadlessStorageBuffer(uint32_t size) noexcept : 
			m_Size(size) {}

		HeadlessStorageBuffer(const HeadlessStorageBuffer&) = delete;
		HeadlessStorageBuffer(HeadlessStorageBuffer&&) noexcept = default;

		HeadlessStorageBuffer& operator=(const HeadlessStorageBuffer&) = delete;
		HeadlessStorageBuffer& operator=(HeadlessStorageBuffer&&) noexcept = default;

		void Bind(int slot, int offset, int size) const {}
		void Unbind(int slot) const {}

		uint32_t GetSize() const { return m_Size; }

		void SetData(const void* data, uint32_t offset, uint32_t size) {}
	};
#pragma endregion


#pragma region HeadlessUniformBuffer
	class HeadlessUniformBuffer
	{
	private:
		uint32_t m_Size;

	public:
		HeadlessUniformBuffer() noexcept = default;
		HeadlessUniformBuffer(uint32_t size) noexcept :
			m_Size(size) {}

		HeadlessUniformBuffer(const HeadlessUniformBuffer&) = delete;
		HeadlessUniformBuffer(HeadlessUniformBuffer&&) noexcept = default;

		HeadlessUniformBuffer& operator=(const HeadlessUniformBuffer&) = delete;
		HeadlessUniformBuffer& operator=(HeadlessUniformBuffer&&) noexcept = default;

		void Bind(int slot, int offset, int size) const {}
		void Unbind(int slot) const {}

		void SetData(const void* data, uint32_t offset, uint32_t size) {}
	};
#pragma endregion
}