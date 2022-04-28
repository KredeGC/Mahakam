#include "mhpch.h"
#include "FrameBuffer.h"

#include "RendererAPI.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Mahakam
{
	//Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferProps& prop)
	MH_DEFINE_FUNC(FrameBuffer::Create, Ref<FrameBuffer>, const FrameBufferProps& prop)
	{
		MH_OVERRIDE_FUNC(FrameBufferCreate, prop);

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			break;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLFrameBuffer>(prop);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};
}