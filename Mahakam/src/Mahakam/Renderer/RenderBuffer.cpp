#include "mhpch.h"
#include "RenderBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRenderBuffer.h"

namespace Mahakam
{
	Ref<RenderBuffer> RenderBuffer::create(uint32_t width, uint32_t height, TextureFormat format)
	{
		switch (RendererAPI::getAPI())
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