#include "Mahakam/mhpch.h"
#include "Buffer.h"

#include "RendererAPI.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam
{
	//Scope<StorageBuffer> StorageBuffer::Create(uint32_t size)
	MH_DEFINE_FUNC(StorageBuffer::Create, Scope<StorageBuffer>, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateScope<StorageBuffer>(std::in_place_type<HeadlessStorageBuffer>, size);
		case RendererAPI::API::OpenGL:
			return CreateScope<StorageBuffer>(std::in_place_type<OpenGLStorageBuffer>, size);
		}

		MH_BREAK("Unknown renderer API!");

		return nullptr;
	};

	//Scope<UniformBuffer> UniformBuffer::Create(uint32_t size)
	MH_DEFINE_FUNC(UniformBuffer::Create, Scope<UniformBuffer>, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateScope<UniformBuffer>(std::in_place_type<HeadlessUniformBuffer>, size);
		case RendererAPI::API::OpenGL:
			return CreateScope<UniformBuffer>(std::in_place_type<OpenGLUniformBuffer>, size);
		}

		MH_BREAK("Unknown renderer API!");

		return nullptr;
	};
}