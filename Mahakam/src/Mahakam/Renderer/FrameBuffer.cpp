#include "mhpch.h"
#include "FrameBuffer.h"

#include "RendererAPI.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Mahakam
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferProps& prop)
	{
		MH_OVERRIDE_FUNC(framebufferCreate, prop);

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLFrameBuffer>(prop);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
}