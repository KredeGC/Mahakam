#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Mahakam
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Mahakam::ShaderDataType::None:
			MH_CORE_BREAK("None is not a supported shader type!");
		case Mahakam::ShaderDataType::Float:
			return GL_FLOAT;
		case Mahakam::ShaderDataType::Float2:
			return GL_FLOAT;
		case Mahakam::ShaderDataType::Float3:
			return GL_FLOAT;
		case Mahakam::ShaderDataType::Float4:
			return GL_FLOAT;
		case Mahakam::ShaderDataType::Mat3:
			return GL_FLOAT;
		case Mahakam::ShaderDataType::Mat4:
			return GL_FLOAT;
		case Mahakam::ShaderDataType::Int:
			return GL_INT;
		case Mahakam::ShaderDataType::Int2:
			return GL_INT;
		case Mahakam::ShaderDataType::Int3:
			return GL_INT;
		case Mahakam::ShaderDataType::Int4:
			return GL_INT;
		case Mahakam::ShaderDataType::Bool:
			return GL_BOOL;
		}

		MH_CORE_BREAK("Unknown shader data type!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray(uint32_t vertexCount) : vertexCount(vertexCount)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glCreateVertexArrays(1, &rendererID));
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteVertexArrays(1, &rendererID));
	}

	void OpenGLVertexArray::Bind() const
	{
		MH_GL_CALL(glBindVertexArray(rendererID));
	}

	void OpenGLVertexArray::Unbind() const
	{
		MH_GL_CALL(glBindVertexArray(0));
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& buffer, bool interleave)
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(buffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

		MH_GL_CALL(glBindVertexArray(rendererID));
		buffer->Bind();

		const auto& layout = buffer->GetLayout();
		for (const auto& element : layout)
		{
			if (element.type != ShaderDataType::None)
			{
				MH_GL_CALL(glEnableVertexAttribArray(vertexBufferIndex));
				GLenum type = ShaderDataTypeToOpenGLBaseType(element.type);
				if (type == GL_INT)
				{
					MH_GL_CALL(glVertexAttribIPointer(vertexBufferIndex,
						element.GetComponentCount(),
						type,
						0,
						(const void*)(uintptr_t)(vertexCount * element.offset)));
				}
				else
				{
					MH_GL_CALL(glVertexAttribPointer(vertexBufferIndex,
						element.GetComponentCount(),
						type,
						element.normalized ? GL_TRUE : GL_FALSE,
						0,
						(const void*)(uintptr_t)(vertexCount * element.offset)));
				}
			}
			vertexBufferIndex++;
		}

		vertexBuffers.push_back(buffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& buffer)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glBindVertexArray(rendererID));
		buffer->Bind();
		indexBuffer = buffer;
	}
}