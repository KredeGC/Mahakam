#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Mahakam
{
#pragma region Camera
	class Camera
	{
	public:
		virtual const glm::mat4& getViewMatrix() const = 0;
		virtual const glm::mat4& getProjectionMatrix() const = 0;
		virtual const glm::mat4& getViewProjectionMatrix() const = 0;
	};
#pragma endregion


#pragma region OrthographicCamera
	class OrthographicCamera : public Camera
	{
	private:
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::mat4 viewProjectionMatrix;

		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };

		void recalculateViewMatrix();

	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void setPosition(const glm::vec3& pos);
		void setRotation(const glm::quat& rot);

		const glm::vec3& getPosition() const;
		const glm::quat& getRotation() const;

		const glm::mat4& getViewMatrix() const { return viewMatrix; }
		const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return viewProjectionMatrix; }
	};
#pragma endregion


#pragma region PerspectiveCamera
	class PerspectiveCamera : public Camera
	{
	private:
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::mat4 viewProjectionMatrix;

		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };

		void recalculateViewMatrix();

	public:
		PerspectiveCamera(float ratio, float nearPlane, float farPlane);

		void setPosition(const glm::vec3& pos);
		void setRotation(const glm::quat& rot);

		const glm::vec3& getPosition() const;
		const glm::quat& getRotation() const;

		const glm::mat4& getViewMatrix() const { return viewMatrix; }
		const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return viewProjectionMatrix; }
	};
#pragma endregion
}