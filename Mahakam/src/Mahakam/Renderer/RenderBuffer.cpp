#include "mhpch.h"
#include "RenderBuffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRenderBuffer.h"

namespace Mahakam
{
	Ref<RenderBuffer> RenderBuffer::create(uint32_t width, uint32_t height, TextureFormat format)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLRenderBuffer>(width, height, format);
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

		return nullptr;
	}
}