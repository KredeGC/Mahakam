#include "mhpch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Mahakam
{
	Ref<VertexArray> VertexArray::create()
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexArray>();
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

		return nullptr;
	}
}