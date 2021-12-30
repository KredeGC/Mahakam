#include "mhpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace Mahakam
{
#pragma region OrthographicCamera
	void OrthographicCamera::recalculateViewMatrix()
	{
		viewMatrix = glm::inverse(transform.getModelMatrix());
		viewProjectionMatrix = projectionMatrix * viewMatrix;

		matrixBuffer->setData(&viewMatrix, 0, sizeof(glm::mat4));
		matrixBuffer->setData(&transform.getPosition(), sizeof(glm::mat4) * 2, sizeof(glm::vec3));
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), viewMatrix(1.0f)
	{
		matrixBuffer = UniformBuffer::create(sizeof(glm::vec3) + sizeof(glm::mat4) * 2);
		matrixBuffer->setData(&projectionMatrix, sizeof(glm::vec3) + sizeof(glm::mat4), sizeof(glm::mat4));

		recalculateViewMatrix();
	}

	void OrthographicCamera::setPosition(const glm::vec3& pos)
	{
		transform.setPosition(pos);
		recalculateViewMatrix();
	}

	void OrthographicCamera::setRotation(const glm::quat& rot)
	{
		transform.setRotation(rot);
		recalculateViewMatrix();
	}
#pragma endregion


#pragma region PerspectiveCamera
	void PerspectiveCamera::recalculateViewMatrix()
	{
		viewMatrix = glm::inverse(transform.getModelMatrix());
		viewProjectionMatrix = projectionMatrix * viewMatrix;

		matrixBuffer->setData(&viewMatrix, 0, sizeof(glm::mat4));
		matrixBuffer->setData(&transform.getPosition(), sizeof(glm::mat4) * 2, sizeof(glm::vec3));
	}

	PerspectiveCamera::PerspectiveCamera(float fov, float ratio, float nearPlane, float farPlane)
		: projectionMatrix(glm::perspective(fov, ratio, nearPlane, farPlane)), viewMatrix(1.0f)
	{
		matrixBuffer = UniformBuffer::create(sizeof(glm::vec3) + sizeof(glm::mat4) * 2);
		matrixBuffer->setData(&projectionMatrix, sizeof(glm::mat4), sizeof(glm::mat4));
		
		recalculateViewMatrix();
	}

	void PerspectiveCamera::setPosition(const glm::vec3& pos)
	{
		transform.setPosition(pos);
		recalculateViewMatrix();
	}

	void PerspectiveCamera::setRotation(const glm::quat& rot)
	{
		transform.setRotation(rot);
		recalculateViewMatrix();
	}
#pragma endregion
}