#include "mhpch.h"
#include "Buffer.h"

#include "RendererAPI.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Mahakam
{
#pragma region UniformBuffer
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size)
	{
		MH_OVERRIDE_FUNC(uniformBufferCreate, size);

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLUniformBuffer>(size);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
#pragma endregion


#pragma region StorageBuffer
	Ref<StorageBuffer> StorageBuffer::Create(uint32_t size)
	{
		MH_OVERRIDE_FUNC(storageBufferCreate, size);

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLStorageBuffer>(size);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
#pragma endregion
}