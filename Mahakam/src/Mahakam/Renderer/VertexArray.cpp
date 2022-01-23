#include "mhpch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Mahakam
{
	Ref<VertexArray> VertexArray::create(uint32_t vertexCount)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexArray>(vertexCount);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
}