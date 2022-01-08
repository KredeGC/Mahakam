#pragma once

#include "Buffer.h"

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace Mahakam
{
	class Camera
	{
	private:
		glm::mat4 projectionMatrix;

		bool perspective = true;
		float nearZ, farZ;
		float size = 0;
		float fov = 0, ratio = 0;

		void recalculateProjectionMatrix()
		{
			if (perspective)
				projectionMatrix = glm::perspective(fov, ratio, nearZ, farZ);
			else
				projectionMatrix = glm::ortho(-ratio * size / 2, ratio * size / 2, -size / 2, size / 2, nearZ, farZ);
		}

	public:
		Camera() : projectionMatrix({ 1.0f }), nearZ(0), farZ(0) {}

		Camera(bool perspective, float fov, float ratio, float nearPlane = 0.03f, float farPlane = 1000.0f)
			: perspective(perspective)
		{
			if (perspective)
				setPerspective(fov, ratio, nearPlane, farPlane);
			else
				setOrthographic(fov, ratio, nearPlane, farPlane);
		}

		Camera(const glm::mat4& projection) : projectionMatrix(projection), nearZ(0), farZ(0) {}

		void setPerspective(float fov, float ratio, float nearPlane = 0.03f, float farPlane = 1000.0f)
		{
			perspective = true;
			this->fov = fov;
			this->ratio = ratio;
			nearZ = nearPlane;
			farZ = farPlane;
			recalculateProjectionMatrix();
		}

		void setOrthographic(float size, float ratio, float nearPlane = 0.03f, float farPlane = 1000.0f)
		{
			perspective = false;
			this->size = size;
			this->ratio = ratio;
			nearZ = nearPlane;
			farZ = farPlane;
			recalculateProjectionMatrix();
		}

		void setFov(float f) { fov = f; recalculateProjectionMatrix(); }

		void setSize(float s) { size = s; recalculateProjectionMatrix(); }

		void setRatio(float r) { ratio = r; recalculateProjectionMatrix(); }

		void setNearPlane(float nearPlane) { nearZ = nearPlane; recalculateProjectionMatrix(); }

		void setFarPlane(float farPlane) { nearZ = farPlane; recalculateProjectionMatrix(); }

		void setProjectionMatrix(const glm::mat4& projection) { projectionMatrix = projection; }

		const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
	};
}