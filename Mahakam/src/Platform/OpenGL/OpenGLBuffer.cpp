#include "Mahakam/mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLBuffer.h"

#include "Mahakam/Core/Profiler.h"

#include <glad/gl.h>

namespace Mahakam
{
#pragma region OpenGLStorageBuffer
	OpenGLStorageBuffer::OpenGLStorageBuffer() noexcept :
		m_RendererID(0),
		m_Size(0) {}

	OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t size) noexcept :
		m_Size(size)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glGenBuffers(1, &m_RendererID));
		MH_GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID));
		MH_GL_CALL(glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
	}

	OpenGLStorageBuffer::OpenGLStorageBuffer(OpenGLStorageBuffer&& other) noexcept :
		m_RendererID(other.m_RendererID),
		m_Size(other.m_Size)
	{
		other.m_RendererID = 0;
		other.m_Size = 0;
	}

	OpenGLStorageBuffer::~OpenGLStorageBuffer()
	{
		release();
	}

	OpenGLStorageBuffer& OpenGLStorageBuffer::operator=(OpenGLStorageBuffer&& other) noexcept
	{
		release();

		m_RendererID = other.m_RendererID;
		m_Size = other.m_Size;

		other.m_RendererID = 0;
		other.m_Size = 0;

		return *this;
	}

	void OpenGLStorageBuffer::Bind(int slot, int offset, int size) const noexcept
	{
		if (!offset && !size)
			MH_GL_CALL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_RendererID))
		else
			MH_GL_CALL(glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slot, m_RendererID, offset, size > 0 ? size : m_Size))
	}

	void OpenGLStorageBuffer::Unbind(int slot) const noexcept
	{
		MH_GL_CALL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, 0));
	}

	uint32_t OpenGLStorageBuffer::GetSize() const noexcept
	{
		return m_Size;
	}

	void OpenGLStorageBuffer::SetData(const void* data, uint32_t offset, uint32_t size) noexcept
	{
		MH_GL_CALL(glNamedBufferSubData(m_RendererID, offset, size, data));
	}

	void OpenGLStorageBuffer::release() noexcept
	{
		MH_PROFILE_FUNCTION();

		if (m_RendererID != 0)
			MH_GL_CALL(glDeleteBuffers(1, &m_RendererID));
	}
#pragma endregion


#pragma region OpenGLUniformBuffer
	OpenGLUniformBuffer::OpenGLUniformBuffer() noexcept :
		m_RendererID(0),
		m_Size(0) {}

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size) noexcept :
		m_Size(size)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glGenBuffers(1, &m_RendererID));
		MH_GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID));
		MH_GL_CALL(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(OpenGLUniformBuffer&& other) noexcept :
		m_RendererID(other.m_RendererID),
		m_Size(other.m_Size)
	{
		other.m_RendererID = 0;
		other.m_Size = 0;
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		release();
	}

	OpenGLUniformBuffer& OpenGLUniformBuffer::operator=(OpenGLUniformBuffer&& other) noexcept
	{
		release();

		m_RendererID = other.m_RendererID;
		m_Size = other.m_Size;

		other.m_RendererID = 0;
		other.m_Size = 0;

		return *this;
	}

	void OpenGLUniformBuffer::Bind(int slot, int offset, int size) const noexcept
	{
		if (!offset && !size)
			MH_GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, slot, m_RendererID))
		else
			MH_GL_CALL(glBindBufferRange(GL_UNIFORM_BUFFER, slot, m_RendererID, offset, size > 0 ? size : m_Size))
	}

	void OpenGLUniformBuffer::Unbind(int slot) const noexcept
	{
		MH_GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, slot, 0));
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t offset, uint32_t size) noexcept
	{
		//MH_PROFILE_FUNCTION();

		//MH_GL_CALL(glNamedBufferSubData(m_RendererID, offset, size, data));

		//MH_GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));

		/*glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
		void* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		memcpy(ptr, data, size);
		glUnmapBuffer(GL_UNIFORM_BUFFER);*/

		void* ptr;
		MH_GL_CALL(ptr = glMapNamedBufferRange(m_RendererID, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT));
		memcpy(ptr, data, size);
		MH_GL_CALL(glUnmapNamedBuffer(m_RendererID));
	}

	void OpenGLUniformBuffer::release() noexcept
	{
		MH_PROFILE_FUNCTION();

		if (m_RendererID != 0)
			MH_GL_CALL(glDeleteBuffers(1, &m_RendererID));
	}
#pragma endregion
}