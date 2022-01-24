#include "mhpch.h"
#include "Camera.h"

namespace Mahakam
{
	Camera::Camera()
		: projectionMatrix({ 1.0f }), nearZ(0.03f), farZ(1000.0f)
	{
		setPerspective(fov, ratio, nearZ, farZ);
	}

	Camera::Camera(ProjectionType projection, float fov, float ratio, float nearPlane, float farPlane)
		: projectionType(projection)
	{
		if (projectionType == ProjectionType::Perspective)
			setPerspective(fov, ratio, nearPlane, farPlane);
		else
			setOrthographic(fov, ratio, nearPlane, farPlane);
	}

	void Camera::recalculateProjectionMatrix()
	{
		if (changed)
		{
			if (projectionType == ProjectionType::Perspective)
				projectionMatrix = glm::perspective(fov, ratio, nearZ, farZ);
			else
				projectionMatrix = glm::ortho(-ratio * size / 2, ratio * size / 2, -size / 2, size / 2, nearZ, farZ);
		}
	}

	void Camera::setPerspective(float fov, float ratio, float nearPlane, float farPlane)
	{
		projectionType = ProjectionType::Perspective;
		this->fov = fov;
		this->ratio = ratio;
		nearZ = nearPlane;
		farZ = farPlane;
		recalculateProjectionMatrix();
	}

	void Camera::setOrthographic(float size, float ratio, float nearPlane, float farPlane)
	{
		projectionType = ProjectionType::Orthographic;
		this->size = size;
		this->ratio = ratio;
		nearZ = nearPlane;
		farZ = farPlane;
		recalculateProjectionMatrix();
	}
}