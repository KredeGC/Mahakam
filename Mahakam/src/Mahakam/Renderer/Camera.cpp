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

		matrixBuffer->setData(&position, 0, sizeof(glm::vec3));
		matrixBuffer->setData(&viewMatrix, sizeof(glm::vec3), sizeof(glm::mat4));
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), viewMatrix(1.0f)
	{
		matrixBuffer = UniformBuffer::create(sizeof(glm::vec3) + sizeof(glm::mat4) * 2);
		matrixBuffer->setData(&projectionMatrix, sizeof(glm::vec3) + sizeof(glm::mat4), sizeof(glm::mat4));
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

		matrixBuffer->setData(&viewMatrix, 0, sizeof(glm::mat4));
		matrixBuffer->setData(&position, sizeof(glm::mat4) * 2, sizeof(glm::vec3));
	}

	PerspectiveCamera::PerspectiveCamera(float ratio, float nearPlane, float farPlane)
		: projectionMatrix(glm::perspective(45.0f, ratio, nearPlane, farPlane)), viewMatrix(1.0f)
	{
		matrixBuffer = UniformBuffer::create(sizeof(glm::vec3) + sizeof(glm::mat4) * 2);
		matrixBuffer->setData(&projectionMatrix, sizeof(glm::mat4), sizeof(glm::mat4));
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