#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Types.h"

#include "Platform/Headless/HeadlessBuffer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

#include "RendererAPI.h"

#include <cstdint>
#include <variant>

namespace Mahakam
{
	using StorageBufferVariant = std::variant<HeadlessStorageBuffer, OpenGLStorageBuffer>;
	using UniformBufferVariant = std::variant<HeadlessUniformBuffer, OpenGLUniformBuffer>;

	class StorageBuffer
	{
	public:
		StorageBuffer() = default;

		template<typename T>
		StorageBuffer(std::in_place_type_t<T> type, uint32_t size) :
			m_Value(type, size) {}

		void Bind(int slot = 0, int offset = 0, int size = 0)
		{
			SwitchCall(m_Value, [&](auto& buffer) { buffer.Bind(slot, offset, size); });
		}

		void Unbind(int slot = 0)
		{
			SwitchCall(m_Value, [&](auto& buffer) { buffer.Unbind(slot); });
		}

		uint32_t GetSize() const
		{
			return SwitchCall(m_Value, [](auto& buffer) { return buffer.GetSize(); });
		}

		void SetData(const void* data, uint32_t offset = 0, uint32_t size = 0)
		{
			SwitchCall(m_Value, [&](auto& buffer) { buffer.SetData(data, offset, size); });
		}

		MH_DECLARE_FUNC(Create, StorageBuffer, uint32_t size);

	private:
		StorageBufferVariant m_Value;
	};


	class UniformBuffer
	{
	public:
		UniformBuffer() = default;

		template<typename T>
		UniformBuffer(std::in_place_type_t<T> type, uint32_t size) :
			m_Value(type, size) {}

		void Bind(int slot = 0, int offset = 0, int size = 0) const
		{
			SwitchCall(m_Value, [&](auto& buffer) { buffer.Bind(slot, offset, size); });
		}

		void Unbind(int slot = 0) const
		{
			SwitchCall(m_Value, [&](auto& buffer) { buffer.Unbind(slot); });
		}

		void SetData(const void* data, uint32_t offset = 0, uint32_t size = 0)
		{
			SwitchCall(m_Value, [&](auto& buffer) { buffer.SetData(data, offset, size); });
		}

		MH_DECLARE_FUNC(Create, UniformBuffer, uint32_t size);

	private:
		UniformBufferVariant m_Value;
	};
}