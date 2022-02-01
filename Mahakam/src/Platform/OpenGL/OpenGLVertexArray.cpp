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
	
	void OpenGLVertexArray::bind() const
	{
		MH_GL_CALL(glBindVertexArray(rendererID));
	}
	
	void OpenGLVertexArray::unbind() const
	{
		MH_GL_CALL(glBindVertexArray(0));
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& buffer, bool interleave)
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(buffer->getLayout().getElements().size(), "Vertex buffer has no layout!");

		MH_GL_CALL(glBindVertexArray(rendererID));
		buffer->bind();

		const auto& layout = buffer->getLayout();
		if (interleave)
		{
			for (const auto& element : layout)
			{
				MH_GL_CALL(glEnableVertexAttribArray(vertexBufferIndex));
				GLenum type = ShaderDataTypeToOpenGLBaseType(element.type);
				if (type == GL_INT)
				{
					MH_GL_CALL(glVertexAttribIPointer(vertexBufferIndex,
						element.getComponentCount(),
						type,
						layout.getStride(),
						(const void*)(uintptr_t)element.offset));
				}
				else
				{
					MH_GL_CALL(glVertexAttribPointer(vertexBufferIndex,
						element.getComponentCount(),
						type,
						element.normalized ? GL_TRUE : GL_FALSE,
						layout.getStride(),
						(const void*)(uintptr_t)element.offset));
				}
				vertexBufferIndex++;
			}
		}
		else
		{
			for (const auto& element : layout)
			{
				MH_GL_CALL(glEnableVertexAttribArray(vertexBufferIndex));
				GLenum type = ShaderDataTypeToOpenGLBaseType(element.type);
				if (type == GL_INT)
				{
					MH_GL_CALL(glVertexAttribIPointer(vertexBufferIndex,
						element.getComponentCount(),
						type,
						0,
						(const void*)(uintptr_t)(vertexCount * element.offset)));
				}
				else
				{
					MH_GL_CALL(glVertexAttribPointer(vertexBufferIndex,
						element.getComponentCount(),
						type,
						element.normalized ? GL_TRUE : GL_FALSE,
						0,
						(const void*)(uintptr_t)(vertexCount * element.offset)));
				}
				vertexBufferIndex++;
			}
		}

		vertexBuffers.push_back(buffer);
	}
	
	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& buffer)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glBindVertexArray(rendererID));
		buffer->bind();
		indexBuffer = buffer;
	}
}