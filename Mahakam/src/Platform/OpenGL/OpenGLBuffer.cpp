#include "mhpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Mahakam
{
#pragma region VertexBuffer
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		MH_PROFILE_FUNCTION();

		glCreateBuffers(1, &rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const char* vertices, uint32_t size)
	{
		MH_PROFILE_FUNCTION();

		glCreateBuffers(1, &rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		MH_PROFILE_FUNCTION();

		glDeleteBuffers(1, &rendererID);
	}

	void OpenGLVertexBuffer::bind() const
	{
		MH_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
	}
	
	void OpenGLVertexBuffer::unbind() const
	{
		MH_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::setData(const char* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}

	void OpenGLVertexBuffer::setLayout(const BufferLayout& layout)
	{
		MH_PROFILE_FUNCTION();

		this->layout = layout;
	}

	const BufferLayout& OpenGLVertexBuffer::getLayout() const
	{
		return layout;
	}
#pragma endregion


#pragma region IndexBuffer
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : count(count)
	{
		MH_PROFILE_FUNCTION();

		glCreateBuffers(1, &rendererID);
		glNamedBufferStorage(rendererID, sizeof(uint32_t) * count, indices, GL_MAP_WRITE_BIT);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * count, indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		MH_PROFILE_FUNCTION();

		glDeleteBuffers(1, &rendererID);
	}

	void OpenGLIndexBuffer::bind() const
	{
		MH_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
	}

	void OpenGLIndexBuffer::unbind() const
	{
		MH_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
#pragma endregion


#pragma region OpenGLUniformBuffer
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size) : size(size)
	{
		MH_PROFILE_FUNCTION();

		glCreateBuffers(1, &rendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		MH_PROFILE_FUNCTION();

		glDeleteBuffers(1, &rendererID);
	}

	void OpenGLUniformBuffer::bind(int slot, int offset, int size) const
	{
		MH_PROFILE_FUNCTION();

		glBindBufferRange(GL_UNIFORM_BUFFER, slot, rendererID, offset, size > 0 ? size : this->size);
	}

	void OpenGLUniformBuffer::unbind(int slot) const
	{
		MH_PROFILE_FUNCTION();

		glBindBufferRange(GL_UNIFORM_BUFFER, slot, 0, 0, 0);
	}

	void OpenGLUniformBuffer::setData(const void* data, uint32_t offset, uint32_t size)
	{
		/*glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
		void* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		memcpy(ptr, data, size);
		glUnmapBuffer(GL_UNIFORM_BUFFER);*/

		void* ptr = glMapNamedBufferRange(rendererID, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		memcpy(ptr, data, size);
		glUnmapNamedBuffer(rendererID);
	}
#pragma endregion


#pragma region OpenGLStorageBuffer
	OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t size) : size(size)
	{
		MH_PROFILE_FUNCTION();

		glCreateBuffers(1, &rendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLStorageBuffer::~OpenGLStorageBuffer()
	{
		MH_PROFILE_FUNCTION();

		glDeleteBuffers(1, &rendererID);
	}

	void OpenGLStorageBuffer::bind(int slot, int offset, int size) const
	{
		MH_PROFILE_FUNCTION();

		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, rendererID, offset, size > 0 ? size : this->size);
	}

	void OpenGLStorageBuffer::unbind(int slot) const
	{
		MH_PROFILE_FUNCTION();

		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, 0, 0, 0);
	}
	
	void OpenGLStorageBuffer::setData(const void* data, uint32_t offset, uint32_t size)
	{
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, rendererID);
		//glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

		void* ptr = glMapNamedBufferRange(rendererID, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		memcpy(ptr, data, size);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}
#pragma endregion
}