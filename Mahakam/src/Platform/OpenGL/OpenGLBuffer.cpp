#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Mahakam
{
#pragma region OpenGLUniformBuffer
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size) : size(size)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glGenBuffers(1, &rendererID));
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
		if (!offset && !size)
			MH_GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, slot, rendererID))
		else
			MH_GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, slot, rendererID, offset, size > 0 ? size : this->size))
	}

	void OpenGLUniformBuffer::Unbind(int slot) const
	{
		MH_GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, slot, 0));
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t offset, uint32_t size)
	{
		MH_GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));

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

		MH_GL_CALL(glGenBuffers(1, &rendererID));
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
		if (!offset && !size)
			MH_GL_CALL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, rendererID))
		else
			MH_GL_CALL(glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, rendererID, offset, size > 0 ? size : this->size))
	}

	void OpenGLStorageBuffer::Unbind(int slot) const
	{
		MH_GL_CALL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, 0));
	}

	uint32_t OpenGLStorageBuffer::GetSize() const
	{
		return size;
	}

	void OpenGLStorageBuffer::SetData(const void* data, uint32_t offset, uint32_t size)
	{
		MH_GL_CALL(glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data));

		/*void* ptr = glMapNamedBufferRange(rendererID, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		memcpy(ptr, data, size);
		glUnmapNamedBuffer(rendererID);*/
	}
#pragma endregion
}