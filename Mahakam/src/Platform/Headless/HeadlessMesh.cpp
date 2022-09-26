#include "Mahakam/mhpch.h"
#include "HeadlessMesh.h"

#include "Mahakam/Core/Profiler.h"

namespace Mahakam
{
	HeadlessMesh::HeadlessMesh(uint32_t vertexCount, uint32_t indexCount, const void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices)
		: vertexCount(vertexCount), indexCount(indexCount), indices(new uint32_t[indexCount])
	{
		std::memcpy(this->indices, indices, indexCount * sizeof(uint32_t));

		// Setup vertices
		for (int i = 0; i < BUFFER_ELEMENTS_SIZE; i++)
		{
			if (verts[i])
			{
				const uint32_t elementSize = vertexCount * ShaderDataTypeSize(BUFFER_ELEMENTS[i]);
				vertices[i] = new uint8_t[elementSize]{ 0 };
				std::memcpy(vertices[i], verts[i], elementSize);
			}
		}

		// Calculate bounds
		RecalculateBounds();
	}

	HeadlessMesh::~HeadlessMesh()
	{
		MH_PROFILE_FUNCTION();

		for (int i = 0; i < BUFFER_ELEMENTS_SIZE; i++)
			delete[] vertices[i];

		delete[] indices;
	}

	void HeadlessMesh::Bind() const
	{
		
	}

	void HeadlessMesh::Unbind() const
	{
		
	}

	void HeadlessMesh::RecalculateBounds()
	{
		bounds = Bounds::CalculateBounds(GetPositions(), vertexCount);
	}

	void HeadlessMesh::RecalculateNormals()
	{
		
	}

	void HeadlessMesh::RecalculateTangents()
	{
		
	}

	void HeadlessMesh::SetVertices(int slot, const void* data)
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_BREAK("Changing an active mesh not currently supported!");

		uint32_t elementSize = ShaderDataTypeSize(BUFFER_ELEMENTS[slot]);
		uint32_t size = elementSize * vertexCount;

		// The buffer already exists
		std::memcpy(vertices[slot], data, size);
	}
}