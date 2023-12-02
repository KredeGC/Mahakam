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
			MH_BREAK("None is not a supported shader type!");
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
			MH_BREAK("Unknown shader data type!");
			return 0;
		}
	}

	OpenGLMesh::OpenGLMesh(uint32_t vertexCount, uint32_t indexCount, MeshData&& mesh, const uint32_t* indices) :
		m_MeshData(std::move(mesh)),
		m_VertexCount(vertexCount),
		m_IndexCount(indexCount)
	{
		// Copy vertices into interleavedVertices
		//InterleaveBuffers(verts, BUFFER_ELEMENTS, BUFFER_ELEMENTS_SIZE);

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

		MH_BREAK("Not currently supported");
	}

	void OpenGLMesh::RecalculateTangents()
	{
		MH_PROFILE_FUNCTION();

		MH_BREAK("Not currently supported");
	}

	void OpenGLMesh::SetVertices(int slot, const void* data)
	{
		MH_PROFILE_FUNCTION();

		MH_BREAK("Changing an active mesh not currently supported!");
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
		MH_GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_MeshData.GetVertexData().size(), m_MeshData.GetVertexData().data(), GL_STATIC_DRAW));

		// Create index buffer
		MH_GL_CALL(glGenBuffers(1, &m_IndexBufferID));
		MH_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferID));
		MH_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_MeshData.GetIndexCount() * sizeof(uint32_t), m_MeshData.GetIndices().data(), GL_STATIC_DRAW));

		// Setup vertex buffer layout
		for (auto& [index, value] : m_MeshData.GetOffsets())
		{
			auto& [offset, type] = value;

			GLenum dataType = ShaderDataTypeToOpenGLBaseType(type);
			uint32_t dataTypeSize = ShaderDataTypeSize(type);
			uint32_t componentCount = ShaderDataTypeComponentCount(type);

			MH_GL_CALL(glEnableVertexAttribArray(index));
			if (dataType == GL_INT)
			{
				MH_GL_CALL(glVertexAttribIPointer(index,
					componentCount,
					dataType,
					0,
					(void*)(uintptr_t)offset));
			}
			else
			{
				MH_GL_CALL(glVertexAttribPointer(index,
					componentCount,
					dataType,
					GL_FALSE, // Why normalize it?
					0,
					(void*)(uintptr_t)offset));
			}
		}

		MH_GL_CALL(glBindVertexArray(0));

		MH_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		MH_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}