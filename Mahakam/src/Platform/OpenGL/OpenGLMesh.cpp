#include "Mahakam/mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLMesh.h"

#include "Mahakam/Core/Profiler.h"

#include <glad/gl.h>

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
		default:
			MH_CORE_BREAK("Unknown shader data type!");
			return 0;
		}
	}

	OpenGLMesh::OpenGLMesh(uint32_t vertexCount, uint32_t indexCount, const void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices) :
		m_InterleavedVertices(Allocator::GetAllocator<uint8_t>()),
		m_Vertices{ Allocator::GetAllocator<uint8_t>(), Allocator::GetAllocator<uint8_t>(), Allocator::GetAllocator<uint8_t>(), Allocator::GetAllocator<uint8_t>(), Allocator::GetAllocator<uint8_t>(), Allocator::GetAllocator<uint8_t>(), Allocator::GetAllocator<uint8_t>() },
		m_Indices(Allocator::GetAllocator<uint32_t>()),
		m_VertexCount(vertexCount),
		m_IndexCount(indexCount)
	{
		m_Indices.assign(indices, indices + indexCount);

		// Setup vertices
		for (uint32_t i = 0; i < BUFFER_ELEMENTS_SIZE; i++)
		{
			if (verts[i])
			{
				uint32_t elementSize = vertexCount * ShaderDataTypeSize(BUFFER_ELEMENTS[i]);
				m_Vertices[i].assign(reinterpret_cast<const uint8_t*>(verts[i]), reinterpret_cast<const uint8_t*>(verts[i]) + elementSize);
			}
		}

		// Copy vertices into interleavedVertices
		InterleaveBuffers();

		// Initialize gpu memory
		Init();

		// Calculate bounds
		RecalculateBounds();
	}

	OpenGLMesh::~OpenGLMesh()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteVertexArrays(1, &m_RendererID));

		MH_GL_CALL(glDeleteBuffers(1, &m_VertexBufferID));
		MH_GL_CALL(glDeleteBuffers(1, &m_IndexBufferID));
	}

	void OpenGLMesh::Bind() const
	{
		MH_GL_CALL(glBindVertexArray(m_RendererID));
	}

	void OpenGLMesh::Unbind() const
	{
		MH_GL_CALL(glBindVertexArray(0));
	}

	void OpenGLMesh::RecalculateBounds()
	{
		MH_PROFILE_FUNCTION();

		m_Bounds = Bounds::CalculateBounds(GetPositions(), m_VertexCount);
	}

	void OpenGLMesh::RecalculateNormals()
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_BREAK("Not currently supported");
	}

	void OpenGLMesh::RecalculateTangents()
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_BREAK("Not currently supported");
	}

	void OpenGLMesh::SetVertices(int slot, const void* data)
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_BREAK("Changing an active mesh not currently supported!");
	}

	void OpenGLMesh::Init()
	{
		MH_PROFILE_FUNCTION();

		// Create VAO
		MH_GL_CALL(glGenVertexArrays(1, &m_RendererID));
		MH_GL_CALL(glBindVertexArray(m_RendererID));

		// Create vertex buffer
		MH_GL_CALL(glGenBuffers(1, &m_VertexBufferID));
		MH_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferID));
		MH_GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_InterleavedVertices.size(), m_InterleavedVertices.data(), GL_STATIC_DRAW));

		// Create index buffer
		MH_GL_CALL(glGenBuffers(1, &m_IndexBufferID));
		MH_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferID));
		MH_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(uint32_t), m_Indices.data(), GL_STATIC_DRAW));

		// Setup vertex buffer layout
		size_t vertexBufferOffset = 0;
		for (int i = 0; i < BUFFER_ELEMENTS_SIZE; i++)
		{
			if (!m_Vertices[i].empty())
			{
				ShaderDataType type = BUFFER_ELEMENTS[i];

				GLenum dataType = ShaderDataTypeToOpenGLBaseType(type);
				uint32_t dataTypeSize = ShaderDataTypeSize(type);
				uint32_t componentCount = ShaderDataTypeComponentCount(type);

				MH_GL_CALL(glEnableVertexAttribArray(i));
				if (dataType == GL_INT)
				{
					MH_GL_CALL(glVertexAttribIPointer(i,
						componentCount,
						dataType,
						0,
						(void*)(uintptr_t)vertexBufferOffset));
				}
				else
				{
					MH_GL_CALL(glVertexAttribPointer(i,
						componentCount,
						dataType,
						GL_FALSE, // Why normalize it?
						0,
						(void*)(uintptr_t)vertexBufferOffset));
				}

				vertexBufferOffset += m_Vertices[i].size();
			}
		}

		MH_GL_CALL(glBindVertexArray(0));

		MH_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		MH_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	void OpenGLMesh::InterleaveBuffers()
	{
		size_t totalSize = 0;
		for (uint32_t i = 0; i < BUFFER_ELEMENTS_SIZE; i++)
		{
			if (!m_Vertices[i].empty())
				totalSize += m_Vertices[i].size();
		}

		m_InterleavedVertices.resize(totalSize);

		size_t dstOffset = 0;
		for (int i = 0; i < BUFFER_ELEMENTS_SIZE; i++)
		{
			if (!m_Vertices[i].empty())
			{
				std::memcpy(m_InterleavedVertices.begin() + dstOffset, m_Vertices[i].begin(), m_Vertices[i].size());
				dstOffset += m_Vertices[i].size();
			}
		}
	}
}