#include "Mahakam/mhpch.h"
#include "HeadlessBuffer.h"

namespace Mahakam
{
#pragma region HeadlessUniformBuffer
	HeadlessUniformBuffer::HeadlessUniformBuffer(uint32_t size)
        : m_Size(size), m_Data(new uint8_t[size]) {}

	HeadlessUniformBuffer::~HeadlessUniformBuffer()
	{
		delete[] m_Data;
	}

	void HeadlessUniformBuffer::Bind(int slot, int offset, int size) const {}

	void HeadlessUniformBuffer::Unbind(int slot) const {}

	void HeadlessUniformBuffer::SetData(const void* data, uint32_t offset, uint32_t size)
	{
		memcpy(m_Data + offset, data, size);
	}
#pragma endregion


#pragma region HeadlessStorageBuffer
	HeadlessStorageBuffer::HeadlessStorageBuffer(uint32_t size)
        : m_Size(size), m_Data(new uint8_t[size]) {}

	HeadlessStorageBuffer::~HeadlessStorageBuffer()
    {
		delete[] m_Data;
    }

	void HeadlessStorageBuffer::Bind(int slot, int offset, int size) const {}

	void HeadlessStorageBuffer::Unbind(int slot) const {}

	uint32_t HeadlessStorageBuffer::GetSize() const
	{
		return m_Size;
	}

	void HeadlessStorageBuffer::SetData(const void* data, uint32_t offset, uint32_t size)
	{
		memcpy(m_Data + offset, data, size);
	}
#pragma endregion
}