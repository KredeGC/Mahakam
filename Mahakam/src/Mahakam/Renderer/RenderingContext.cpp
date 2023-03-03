#include "Mahakam/mhpch.h"
#include "RenderingContext.h"

#include "RendererAPI.h"

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Log.h"

#include "Platform/Headless/HeadlessContext.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Mahakam
{
	Scope<RenderingContext> RenderingContext::Create(void* window, void* proc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateScope<HeadlessContext>(window, proc);
		case RendererAPI::API::OpenGL:
			return CreateScope<OpenGLContext>(window, proc);
		}

		MH_BREAK("Unknown renderer API!");

		return nullptr;
	}
}