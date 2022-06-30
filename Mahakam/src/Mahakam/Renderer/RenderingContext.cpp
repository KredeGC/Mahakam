#include "mhpch.h"
#include "RenderingContext.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Mahakam
{
	RenderingContext* RenderingContext::Create(void* window, void* proc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLContext(window, proc);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
}