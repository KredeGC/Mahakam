#include "mhpch.h"
#include "Camera.h"

namespace Mahakam
{
	Camera::Camera()
		: projectionMatrix({ 1.0f }), nearZ(0.03f), farZ(1000.0f)
	{
		MH_PROFILE_FUNCTION();

		SetPerspective(fov, nearZ, farZ);
	}

	Camera::Camera(ProjectionType projection, float fov, float nearPlane, float farPlane)
		: projectionType(projection)
	{
		MH_PROFILE_FUNCTION();

		if (projectionType == ProjectionType::Perspective)
			SetPerspective(fov, nearPlane, farPlane);
		else
			SetOrthographic(fov, nearPlane, farPlane);
	}

	void Camera::RecalculateProjectionMatrix()
	{
		if (changed)
		{
			if (projectionType == ProjectionType::Perspective)
				projectionMatrix = glm::perspective(fov, ratio, nearZ, farZ);
			else
				projectionMatrix = glm::ortho(-ratio * size / 2, ratio * size / 2, -size / 2, size / 2, nearZ, farZ);
		}
	}

	void Camera::SetPerspective(float fov, float nearPlane, float farPlane)
	{
		changed = true;
		projectionType = ProjectionType::Perspective;
		this->fov = fov;
		this->nearZ = nearPlane;
		this->farZ = farPlane;
		RecalculateProjectionMatrix();
	}

	void Camera::SetOrthographic(float size, float nearPlane, float farPlane)
	{
		changed = true;
		projectionType = ProjectionType::Orthographic;
		this->size = size;
		this->nearZ = nearPlane;
		this->farZ = farPlane;
		RecalculateProjectionMatrix();
	}
}