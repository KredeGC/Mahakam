#include "mhpch.h"
#include "Camera.h"

namespace Mahakam
{
	Camera::Camera()
		: projectionMatrix({ 1.0f }), nearZ(0.03f), farZ(1000.0f)
	{
		SetPerspective(fov, ratio, nearZ, farZ);
	}

	Camera::Camera(ProjectionType projection, float fov, float ratio, float nearPlane, float farPlane, const std::initializer_list<RenderPass*>& renderpasses)
		: projectionType(projection), renderpasses(renderpasses)
	{
		if (projectionType == ProjectionType::Perspective)
			SetPerspective(fov, ratio, nearPlane, farPlane);
		else
			SetOrthographic(fov, ratio, nearPlane, farPlane);
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

	void Camera::SetPerspective(float fov, float ratio, float nearPlane, float farPlane)
	{
		projectionType = ProjectionType::Perspective;
		this->fov = fov;
		this->ratio = ratio;
		nearZ = nearPlane;
		farZ = farPlane;
		RecalculateProjectionMatrix();
	}

	void Camera::SetOrthographic(float size, float ratio, float nearPlane, float farPlane)
	{
		projectionType = ProjectionType::Orthographic;
		this->size = size;
		this->ratio = ratio;
		nearZ = nearPlane;
		farZ = farPlane;
		RecalculateProjectionMatrix();
	}

	void Camera::SetRenderPasses(const std::vector<RenderPass*>& renderpasses)
	{
		if (!this->renderpasses.empty())
		{
			for (auto& pass : renderpasses)
				delete pass;
		}

		this->renderpasses = renderpasses;
	}
}