#pragma once

#include "ShaderDataTypes.h"

#include <vector>

namespace Mahakam
{
#pragma region UniformBuffer
	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {}

		virtual void Bind(int slot = 0, int offset = 0, int size = 0) const = 0;
		virtual void Unbind(int slot = 0) const = 0;

		virtual void SetData(const void* data, uint32_t offset = 0, uint32_t size = 0) = 0;

		static Ref<UniformBuffer> Create(uint32_t size);
	};
#pragma endregion


#pragma region StorageBuffer
	class StorageBuffer
	{
	public:
		virtual ~StorageBuffer() {}

		virtual void Bind(int slot = 0, int offset = 0, int size = 0) const = 0;
		virtual void Unbind(int slot = 0) const = 0;

		virtual uint32_t GetSize() const = 0;

		virtual void SetData(const void* data, uint32_t offset = 0, uint32_t size = 0) = 0;

		static Ref<StorageBuffer> Create(uint32_t size);
	};
#pragma endregion
}