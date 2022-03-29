#include "mhpch.h"
#include "RenderBuffer.h"

#include "RendererAPI.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLRenderBuffer.h"

namespace Mahakam
{
	Ref<RenderBuffer> RenderBuffer::Create(uint32_t width, uint32_t height, TextureFormat format)
	{
#if MH_DEBUG
		if (SharedLibrary::renderBufferCreate != nullptr)
			return SharedLibrary::renderBufferCreate(width, height, format);
#endif

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLRenderBuffer>(width, height, format);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
}