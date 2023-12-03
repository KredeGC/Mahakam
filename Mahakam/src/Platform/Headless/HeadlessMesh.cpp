#include "Mahakam/mhpch.h"
#include "HeadlessMesh.h"

#include "Mahakam/Core/Profiler.h"

namespace Mahakam
{
	HeadlessMesh::HeadlessMesh(MeshData&& mesh)
		: m_MeshData(std::move(mesh))
	{
		// Calculate bounds
		RecalculateBounds();
	}

	void HeadlessMesh::Bind() const
	{
		
	}

	void HeadlessMesh::Unbind() const
	{
		
	}

	void HeadlessMesh::RecalculateBounds()
	{
		m_Bounds = Bounds::CalculateBounds(GetPositions(), m_MeshData.GetVertexCount());
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

		MH_BREAK("Changing an active mesh not currently supported!");
	}
}