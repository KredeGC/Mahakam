#include "Mahakam/mhpch.h"
#include "RenderBuffer.h"

#include "RendererAPI.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLRenderBuffer.h"

namespace Mahakam
{
	//Ref<RenderBuffer> RenderBuffer::CreateImpl(uint32_t width, uint32_t height, TextureFormat format)
	MH_DEFINE_FUNC(RenderBuffer::CreateImpl, Asset<RenderBuffer>, uint32_t width, uint32_t height, TextureFormat format)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return Asset<OpenGLRenderBuffer>(CreateRef<OpenGLRenderBuffer>(width, height, format));
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};
}