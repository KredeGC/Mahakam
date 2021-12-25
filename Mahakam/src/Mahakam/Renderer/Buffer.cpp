#include "mhpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Mahakam
{
	void BufferLayout::calculateOffsetAndStride()
	{
		uint32_t offset = 0;
		stride = 0;
		for (auto& element : elements)
		{
			element.offset = offset;
			offset += element.size;
			stride += element.size;
		}
	}


#pragma region VertexBuffer
	Ref<VertexBuffer> VertexBuffer::create(const char* vertices, uint32_t size)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::create(uint32_t size)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(size);
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

		return nullptr;
	}
#pragma endregion


#pragma region IndexBuffer
	Ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLIndexBuffer>(indices, count);
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

		return nullptr;
	}
#pragma endregion


#pragma region UniformBuffer
	Ref<UniformBuffer> UniformBuffer::create(uint32_t size)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLUniformBuffer>(size);
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

		return nullptr;
	}
#pragma endregion
}