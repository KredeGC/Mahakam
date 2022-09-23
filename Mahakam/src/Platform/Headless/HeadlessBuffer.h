#pragma once
#include "Mahakam/Renderer/Buffer.h"

namespace Mahakam
{
#pragma region HeadlessUniformBuffer
	class HeadlessUniformBuffer : public UniformBuffer
	{
	private:
		uint32_t m_Size;
        uint8_t* m_Data = nullptr;

	public:
		HeadlessUniformBuffer(uint32_t size);
		virtual ~HeadlessUniformBuffer() override;

		virtual void Bind(int slot, int offset, int size) const override;
		virtual void Unbind(int slot) const override;

		virtual void SetData(const void* data, uint32_t offset, uint32_t size) override;
	};
#pragma endregion


#pragma region HeadlessStorageBuffer
	class HeadlessStorageBuffer : public StorageBuffer
	{
	private:
		uint32_t m_Size;
        uint8_t* m_Data = nullptr;

	public:
		HeadlessStorageBuffer(uint32_t size);
		virtual ~HeadlessStorageBuffer() override;

		virtual void Bind(int slot, int offset, int size) const override;
		virtual void Unbind(int slot) const override;

		virtual uint32_t GetSize() const override;

		virtual void SetData(const void* data, uint32_t offset, uint32_t size) override;
	};
#pragma endregion
}