#include "Mahakam/mhpch.h"
#include "Buffer.h"

#include "RendererAPI.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/Headless/HeadlessBuffer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Mahakam
{
	//Ref<UniformBuffer> UniformBuffer::Create(uint32_t size)
	MH_DEFINE_FUNC(UniformBuffer::Create, Ref<UniformBuffer>, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateRef<HeadlessUniformBuffer>(size);
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLUniformBuffer>(size);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};


	//Ref<StorageBuffer> StorageBuffer::Create(uint32_t size)
	MH_DEFINE_FUNC(StorageBuffer::Create, Ref<StorageBuffer>, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateRef<HeadlessStorageBuffer>(size);
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLStorageBuffer>(size);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};
}