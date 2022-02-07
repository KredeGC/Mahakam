#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLMesh.h"

namespace Mahakam
{
	static uint32_t ShaderDataTypeComponentCount(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:
			return 1;
		case ShaderDataType::Float2:
			return 2;
		case ShaderDataType::Float3:
			return 3;
		case ShaderDataType::Float4:
			return 4;
		case ShaderDataType::Mat3:
			return 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4;
		case ShaderDataType::Int:
			return 1;
		case ShaderDataType::Int2:
			return 2;
		case ShaderDataType::Int3:
			return 3;
		case ShaderDataType::Int4:
			return 4;
		case ShaderDataType::Bool:
			return 1;
		}

		MH_CORE_BREAK("Unknown shader data type!");
		return 0;
	}

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:
			return 4;
		case ShaderDataType::Float2:
			return 8;
		case ShaderDataType::Float3:
			return 12;
		case ShaderDataType::Float4:
			return 16;
		case ShaderDataType::Mat3:
			return 4 * 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4 * 4;
		case ShaderDataType::Int:
			return 4;
		case ShaderDataType::Int2:
			return 8;
		case ShaderDataType::Int3:
			return 12;
		case ShaderDataType::Int4:
			return 16;
		case ShaderDataType::Bool:
			return 1;
		}

		MH_CORE_BREAK("Unknown shader data type!");
		return 0;
	}

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

	OpenGLMesh::OpenGLMesh(uint32_t vertexCount, uint32_t indexCount, void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices)
		: vertexCount(vertexCount), indexCount(indexCount), indices(new uint32_t[indexCount])
	{
		memcpy(this->indices, indices, indexCount * sizeof(uint32_t));

		MH_PROFILE_FUNCTION();

		// Create VAO
		MH_GL_CALL(glCreateVertexArrays(1, &rendererID));
		MH_GL_CALL(glBindVertexArray(rendererID));

		// Setup vertices
		uint32_t size = 0;
		for (int i = 0; i < BUFFER_ELEMENTS_SIZE; i++)
		{
			if (verts[i])
			{
				vertices[i] = verts[i];
				size += vertexCount * ShaderDataTypeSize(BUFFER_ELEMENTS[i]);
			}
		}

		// Copy vertices into interleavedVertices
		InterleaveBuffers();

		// Create vertex buffer
		MH_GL_CALL(glCreateBuffers(1, &vertexBufferID));
		MH_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
		MH_GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, interleavedVertices, GL_STATIC_DRAW));

		// Create index buffer
		MH_GL_CALL(glCreateBuffers(1, &indexBufferID));
		MH_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));
		MH_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), this->indices, GL_STATIC_DRAW));

		// Setup vertex buffer layout
		uint32_t vertexBufferOffset = 0;
		uint32_t vertexBufferIndex = 0;
		for (int i = 0; i < BUFFER_ELEMENTS_SIZE; i++)
		{
			if (verts[i] != nullptr)
			{
				const ShaderDataType& type = BUFFER_ELEMENTS[i];

				GLenum dataType = ShaderDataTypeToOpenGLBaseType(type);
				uint32_t dataTypeSize = ShaderDataTypeSize(type);
				uint32_t componentCount = ShaderDataTypeComponentCount(type);

				MH_GL_CALL(glEnableVertexAttribArray(vertexBufferIndex));
				if (dataType == GL_INT)
				{
					MH_GL_CALL(glVertexAttribIPointer(vertexBufferIndex,
						componentCount,
						dataType,
						0,
						(const void*)(uintptr_t)vertexBufferOffset));
				}
				else
				{
					MH_GL_CALL(glVertexAttribPointer(vertexBufferIndex,
						componentCount,
						dataType,
						GL_FALSE, // Why normalize it?
						0,
						(const void*)(uintptr_t)vertexBufferOffset));
				}

				vertexBufferOffset += vertexCount * dataTypeSize;
			}
			vertexBufferIndex++;
		}

		RecalculateBounds();
	}

	OpenGLMesh::~OpenGLMesh()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteVertexArrays(1, &rendererID));

		MH_GL_CALL(glDeleteBuffers(1, &vertexBufferID));
		MH_GL_CALL(glDeleteBuffers(1, &indexBufferID));
	}

	void OpenGLMesh::Bind() const
	{
		MH_GL_CALL(glBindVertexArray(rendererID));
	}

	void OpenGLMesh::Unbind() const
	{
		MH_GL_CALL(glBindVertexArray(0));
	}

	void OpenGLMesh::RecalculateBounds()
	{
		bounds = Mesh::CalculateBounds(GetPositions(), vertexCount);
	}

	void OpenGLMesh::RecalculateNormals()
	{
		MH_CORE_BREAK("Not currently supported");
	}

	void OpenGLMesh::RecalculateTangents()
	{
		MH_CORE_BREAK("Not currently supported");
	}

	void OpenGLMesh::SetVertices(int slot, const void* data)
	{
		MH_PROFILE_FUNCTION();

		uint32_t elementSize = ShaderDataTypeSize(BUFFER_ELEMENTS[slot]);
		uint32_t size = elementSize * vertexCount;

		auto& iter = vertices.find(slot);
		if (iter == vertices.end())
		{
			// The buffer doesn't exist
			void* verts = new char[size];

			memcpy(verts, verts, size);

			vertices[slot] = verts;
		}
		else
		{
			// The buffer already exists
			memcpy(vertices[slot], data, size);

			MH_CORE_BREAK("Changing an active mesh not currently supported!");
		}
	}

	void OpenGLMesh::InterleaveBuffers()
	{
		uint32_t totalSize = 0;
		uint32_t sizes[BUFFER_ELEMENTS_SIZE]{ 0 };
		for (int i = 0; i < BUFFER_ELEMENTS_SIZE; i++)
		{
			auto& iter = vertices.find(i);
			if (iter != vertices.end())
			{
				totalSize += vertexCount * ShaderDataTypeSize(BUFFER_ELEMENTS[i]);
				sizes[i] = ShaderDataTypeSize(BUFFER_ELEMENTS[i]);
			}
		}

		if (interleavedVertices)
			delete[] interleavedVertices;
		interleavedVertices = new char[totalSize];

		uint32_t dstOffset = 0;
		for (auto& vert : vertices)
		{
			uint32_t size = vertexCount * sizes[vert.first];
			memcpy(interleavedVertices + dstOffset, vert.second, size);
			dstOffset += size;
		}
	}
}