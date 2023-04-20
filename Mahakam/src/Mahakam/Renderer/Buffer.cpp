#include "Mahakam/mhpch.h"
#include "Buffer.h"

#include "RendererAPI.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/Headless/HeadlessBuffer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Mahakam
{
	//Scope<UniformBuffer> UniformBuffer::Create(uint32_t size)
	MH_DEFINE_FUNC(UniformBuffer::Create, Scope<UniformBuffer>, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateScope<HeadlessUniformBuffer>(size);
		case RendererAPI::API::OpenGL:
			return CreateScope<OpenGLUniformBuffer>(size);
		}

		MH_BREAK("Unknown renderer API!");

		return nullptr;
	};


	//Scope<StorageBuffer> StorageBuffer::Create(uint32_t size)
	MH_DEFINE_FUNC(StorageBuffer::Create, Scope<StorageBuffer>, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateScope<HeadlessStorageBuffer>(size);
		case RendererAPI::API::OpenGL:
			return CreateScope<OpenGLStorageBuffer>(size);
		}

		MH_BREAK("Unknown renderer API!");

		return nullptr;
	};
}