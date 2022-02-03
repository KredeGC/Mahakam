#include "mhpch.h"
#include "VertexArray.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Mahakam
{
	Ref<VertexArray> VertexArray::Create(uint32_t vertexCount)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexArray>(vertexCount);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
}