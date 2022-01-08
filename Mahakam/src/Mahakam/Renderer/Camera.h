#pragma once

#include "Buffer.h"

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace Mahakam
{
	class Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0,
			Orthographic
		};

	private:
		glm::mat4 projectionMatrix;

		bool changed = false;

		ProjectionType projectionType = ProjectionType::Perspective;
		float nearZ, farZ;
		float size = 0;
		float fov = 0, ratio = 0;

	public:
		Camera() : projectionMatrix({ 1.0f }), nearZ(0), farZ(0) {}

		Camera(ProjectionType projection, float fov, float ratio, float nearPlane = 0.03f, float farPlane = 1000.0f)
			: projectionType(projection)
		{
			if (projectionType == ProjectionType::Perspective)
				setPerspective(fov, ratio, nearPlane, farPlane);
			else
				setOrthographic(fov, ratio, nearPlane, farPlane);
		}

		Camera(const glm::mat4& projection) : projectionMatrix(projection), nearZ(0), farZ(0) {}

		void recalculateProjectionMatrix()
		{
			if (changed)
			{
				if (projectionType == ProjectionType::Perspective)
					projectionMatrix = glm::perspective(fov, ratio, nearZ, farZ);
				else
					projectionMatrix = glm::ortho(-ratio * size / 2, ratio * size / 2, -size / 2, size / 2, nearZ, farZ);
			}
		}

		void setPerspective(float fov, float ratio, float nearPlane = 0.03f, float farPlane = 1000.0f)
		{
			projectionType = ProjectionType::Perspective;
			this->fov = fov;
			this->ratio = ratio;
			nearZ = nearPlane;
			farZ = farPlane;
			recalculateProjectionMatrix();
		}

		void setOrthographic(float size, float ratio, float nearPlane = 0.03f, float farPlane = 1000.0f)
		{
			projectionType = ProjectionType::Orthographic;
			this->size = size;
			this->ratio = ratio;
			nearZ = nearPlane;
			farZ = farPlane;
			recalculateProjectionMatrix();
		}

		void setFov(float f) { fov = f; changed = true; }

		float getFov() const { return fov; }

		void setSize(float s) { size = s; changed = true; }

		float getSize() const { return size; }

		void setRatio(float r) { ratio = r; changed = true; }

		float getRatio() const { return ratio; }

		void setNearPlane(float nearPlane) { nearZ = nearPlane; changed = true; }

		float getNearPlane() const { return nearZ; }

		void setFarPlane(float farPlane) { farZ = farPlane; changed = true; }

		float getFarPlane() const { return farZ; }

		void setProjectionType(ProjectionType type) { projectionType = type; changed = true; }

		ProjectionType getProjectionType() const { return projectionType; }

		void setProjectionMatrix(const glm::mat4& projection) { projectionMatrix = projection; }

		const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
	};
}