#include "mhpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace Mahakam
{
	void OrthographicCamera::recalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::mat4(rotation);

		viewMatrix = glm::inverse(transform);
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), viewMatrix(1.0f)
	{
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	void OrthographicCamera::setPosition(const glm::vec3& pos)
	{
		position = pos;
		recalculateViewMatrix();
	}

	void OrthographicCamera::setRotation(const glm::quat& rot)
	{
		rotation = rot;
		recalculateViewMatrix();
	}
	
	const glm::vec3& OrthographicCamera::getPosition() const
	{
		return position;
	}
	
	const glm::quat& OrthographicCamera::getRotation() const
	{
		return rotation;
	}


#pragma region PerspectiveCamera
	void PerspectiveCamera::recalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::mat4(rotation);

		viewMatrix = glm::inverse(transform);
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	PerspectiveCamera::PerspectiveCamera(float ratio, float nearPlane, float farPlane)
		: projectionMatrix(glm::perspective(45.0f, ratio, nearPlane, farPlane)), viewMatrix(1.0f)
	{
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}
	
	void PerspectiveCamera::setPosition(const glm::vec3& pos)
	{
		position = pos;
		recalculateViewMatrix();
	}
	
	void PerspectiveCamera::setRotation(const glm::quat& rot)
	{
		rotation = rot;
		recalculateViewMatrix();
	}
	
	const glm::vec3& PerspectiveCamera::getPosition() const
	{
		return position;
	}
	const glm::quat& PerspectiveCamera::getRotation() const
	{
		return rotation;
	}
#pragma endregion
}