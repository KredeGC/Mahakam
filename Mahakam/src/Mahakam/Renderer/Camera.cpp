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

	void OrthographicCamera::recalculateProjectionMatrix()
	{
		projectionMatrix = glm::ortho(left, right, bottom, top, nearZ, farZ);
		matrixBuffer->setData(&projectionMatrix, sizeof(glm::mat4), sizeof(glm::mat4));
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: left(left), right(right), bottom(bottom), top(top), viewMatrix(1.0f)
	{
		matrixBuffer = UniformBuffer::create(sizeof(glm::vec3) + sizeof(glm::mat4) * 2);

		recalculateProjectionMatrix();
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

	void OrthographicCamera::setNearPlane(float nearZ)
	{
		this->nearZ = nearZ;
		recalculateProjectionMatrix();
	}

	void OrthographicCamera::setFarPlane(float farZ)
	{
		this->farZ = farZ;
		recalculateProjectionMatrix();
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

	void PerspectiveCamera::recalculateProjectionMatrix()
	{
		projectionMatrix = glm::perspective(fov, ratio, nearZ, farZ);
		matrixBuffer->setData(&projectionMatrix, sizeof(glm::mat4), sizeof(glm::mat4));
	}

	PerspectiveCamera::PerspectiveCamera(float fov, float ratio, float nearPlane, float farPlane)
		: fov(fov), ratio(ratio), viewMatrix(1.0f)
	{
		nearZ = nearPlane;
		farZ = farPlane;

		matrixBuffer = UniformBuffer::create(sizeof(glm::vec3) + sizeof(glm::mat4) * 2);
		
		recalculateProjectionMatrix();
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

	void PerspectiveCamera::setFOV(float fov)
	{
		this->fov = fov;
		recalculateProjectionMatrix();
	}

	void PerspectiveCamera::setRatio(float ratio)
	{
		this->ratio = ratio;
		recalculateProjectionMatrix();
	}

	void PerspectiveCamera::setNearPlane(float nearZ)
	{
		this->nearZ = nearZ;
		recalculateProjectionMatrix();
	}

	void PerspectiveCamera::setFarPlane(float farZ)
	{
		this->farZ = farZ;
		recalculateProjectionMatrix();
	}
#pragma endregion
}