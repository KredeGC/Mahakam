#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Mahakam
{
#pragma region VertexBuffer
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glCreateBuffers(1, &rendererID));
		MH_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
		MH_GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const char* vertices, uint32_t size)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glCreateBuffers(1, &rendererID));
		MH_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
		MH_GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteBuffers(1, &rendererID));
	}

	void OpenGLVertexBuffer::Bind() const
	{
		MH_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
	}
	
	void OpenGLVertexBuffer::Unbind() const
	{
		MH_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void OpenGLVertexBuffer::SetData(const char* data, uint32_t size)
	{
		MH_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
		MH_GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
	}

	void OpenGLVertexBuffer::SetLayout(const BufferLayout& layout)
	{
		MH_PROFILE_FUNCTION();

		this->layout = layout;
	}

	const BufferLayout& OpenGLVertexBuffer::GetLayout() const
	{
		return layout;
	}
#pragma endregion


#pragma region IndexBuffer
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : count(count)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glCreateBuffers(1, &rendererID));
		MH_GL_CALL(glNamedBufferStorage(rendererID, sizeof(uint32_t) * count, indices, GL_MAP_WRITE_BIT));
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * count, indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteBuffers(1, &rendererID));
	}

	void OpenGLIndexBuffer::Bind() const
	{
		MH_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID));
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		MH_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
#pragma endregion


#pragma region OpenGLUniformBuffer
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size) : size(size)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glCreateBuffers(1, &rendererID));
		MH_GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, rendererID));
		MH_GL_CALL(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteBuffers(1, &rendererID));
	}

	void OpenGLUniformBuffer::Bind(int slot, int offset, int size) const
	{
		MH_GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, slot, rendererID, offset, size > 0 ? size : this->size));
	}

	void OpenGLUniformBuffer::Unbind(int slot) const
	{
		MH_GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, slot, 0, 0, 0));
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t offset, uint32_t size)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glNamedBufferSubData(rendererID, offset, size, data));

		/*glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
		void* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		memcpy(ptr, data, size);
		glUnmapBuffer(GL_UNIFORM_BUFFER);*/

		/*void* ptr = glMapNamedBufferRange(rendererID, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		memcpy(ptr, data, size);
		glUnmapNamedBuffer(rendererID);*/
	}
#pragma endregion


#pragma region OpenGLStorageBuffer
	OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t size) : size(size)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glCreateBuffers(1, &rendererID));
		MH_GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererID));
		MH_GL_CALL(glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
	}

	OpenGLStorageBuffer::~OpenGLStorageBuffer()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteBuffers(1, &rendererID));
	}

	void OpenGLStorageBuffer::Bind(int slot, int offset, int size) const
	{
		MH_GL_CALL(glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, rendererID, offset, size > 0 ? size : this->size));
	}

	void OpenGLStorageBuffer::Unbind(int slot) const
	{
		MH_GL_CALL(glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, 0, 0, 0));
	}

	uint32_t OpenGLStorageBuffer::GetSize() const
	{
		return size;
	}
	
	void OpenGLStorageBuffer::SetData(const void* data, uint32_t offset, uint32_t size)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glNamedBufferSubData(rendererID, offset, size, data));

		/*void* ptr = glMapNamedBufferRange(rendererID, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		memcpy(ptr, data, size);
		glUnmapNamedBuffer(rendererID);*/
	}
#pragma endregion
}