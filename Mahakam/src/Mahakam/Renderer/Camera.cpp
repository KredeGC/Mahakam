#include "Mahakam/mhpch.h"
#include "Camera.h"

#include "Mahakam/Core/Profiler.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_clip_space.hpp>

namespace Mahakam
{
	Camera::Camera()
		: m_ProjectionMatrix({ 1.0f }), m_NearZ(0.03f), m_FarZ(1000.0f)
	{
		MH_PROFILE_FUNCTION();

		SetPerspective(m_Fov, m_NearZ, m_FarZ);
	}

	Camera::Camera(ProjectionType projection, float fov, float nearPlane, float farPlane)
		: m_ProjectionType(projection)
	{
		MH_PROFILE_FUNCTION();

		if (m_ProjectionType == ProjectionType::Perspective)
			SetPerspective(fov, nearPlane, farPlane);
		else
			SetOrthographic(fov, nearPlane, farPlane);
	}

	void Camera::RecalculateProjectionMatrix()
	{
		if (m_Changed)
		{
			if (m_ProjectionType == ProjectionType::Perspective)
				m_ProjectionMatrix = glm::perspective(m_Fov, m_Ratio, m_NearZ, m_FarZ);
			else
				m_ProjectionMatrix = glm::ortho(-m_Ratio * m_Size / 2, m_Ratio * m_Size / 2, -m_Size / 2, m_Size / 2, m_NearZ, m_FarZ);
		}
	}

	void Camera::SetPerspective(float fov, float nearPlane, float farPlane)
	{
		m_Changed = true;
		m_ProjectionType = ProjectionType::Perspective;
		m_Fov = fov;
		m_NearZ = nearPlane;
		m_FarZ = farPlane;
		RecalculateProjectionMatrix();
	}

	void Camera::SetOrthographic(float size, float nearPlane, float farPlane)
	{
		m_Changed = true;
		m_ProjectionType = ProjectionType::Orthographic;
		m_Size = size;
		m_NearZ = nearPlane;
		m_FarZ = farPlane;
		RecalculateProjectionMatrix();
	}
}