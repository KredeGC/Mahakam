#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Types.h"

namespace Mahakam
{
	class StorageBuffer
	{
	public:
		virtual ~StorageBuffer() = default;

		virtual void Bind(int slot = 0, int offset = 0, int size = 0) const = 0;
		virtual void Unbind(int slot = 0) const = 0;

		virtual uint32_t GetSize() const = 0;

		virtual void SetData(const void* data, uint32_t offset = 0, uint32_t size = 0) = 0;

		MH_DECLARE_FUNC(Create, Scope<StorageBuffer>, uint32_t size);
	};


	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void Bind(int slot = 0, int offset = 0, int size = 0) const = 0;
		virtual void Unbind(int slot = 0) const = 0;

		virtual void SetData(const void* data, uint32_t offset = 0, uint32_t size = 0) = 0;

		MH_DECLARE_FUNC(Create, Scope<UniformBuffer>, uint32_t size);
	};
}