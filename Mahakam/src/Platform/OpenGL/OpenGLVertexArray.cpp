#include "mhpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Mahakam
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Mahakam::ShaderDataType::None:
			return GL_FLOAT;
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

		MH_CORE_ASSERT(false, "Unknown shader data type!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &rendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &rendererID);
	}
	
	void OpenGLVertexArray::bind() const
	{
		glBindVertexArray(rendererID);
	}
	
	void OpenGLVertexArray::unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& buffer)
	{
		MH_CORE_ASSERT(buffer->getLayout().getElements().size(), "Vertex buffer has no layout!");

		glBindVertexArray(rendererID);
		buffer->bind();

		const auto& layout = buffer->getLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(vertexBufferIndex);
			glVertexAttribPointer(vertexBufferIndex,
				element.getComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void*)element.offset);
			vertexBufferIndex++;
		}

		vertexBuffers.push_back(buffer);
	}
	
	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& buffer)
	{
		glBindVertexArray(rendererID);
		buffer->bind();
		indexBuffer = buffer;
	}
}