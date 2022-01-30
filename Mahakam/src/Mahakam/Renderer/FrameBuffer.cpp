#include "mhpch.h"
#include "FrameBuffer.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Mahakam
{
	Ref<FrameBuffer> FrameBuffer::create(const FrameBufferProps& prop)
	{
		switch (RendererAPI::getAPI())
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