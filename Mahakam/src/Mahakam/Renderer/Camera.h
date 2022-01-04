#pragma once

#include "Buffer.h"
#include "Mahakam/Core/Transform.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Mahakam
{
#pragma region Camera
	class Camera
	{
	protected:
		Transform transform;
		float nearZ, farZ;

	public:
		Camera() : transform({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }), nearZ(0.03f), farZ(1000.0f) {}

		virtual const glm::mat4& getViewMatrix() const = 0;
		virtual const glm::mat4& getProjectionMatrix() const = 0;
		virtual const glm::mat4& getViewProjectionMatrix() const = 0;

		virtual void setNearPlane(float nearZ) = 0;
		virtual void setFarPlane(float farZ) = 0;

		virtual const Ref<UniformBuffer>& getMatrixBuffer() const = 0;
	};
#pragma endregion


#pragma region OrthographicCamera
	class OrthographicCamera : public Camera
	{
	protected:
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::mat4 viewProjectionMatrix;

		float left, right, bottom, top;

		Ref<UniformBuffer> matrixBuffer;

		void recalculateViewMatrix();
		void recalculateProjectionMatrix();

	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void setPosition(const glm::vec3& pos);
		void setRotation(const glm::quat& rot);

		inline const glm::vec3& getPosition() const { return transform.getPosition(); }
		inline const glm::quat& getRotation() const { return transform.getRotation(); }

		inline const glm::vec3& getForward() const { return transform.getForward(); }
		inline const glm::vec3& getRight() const { return transform.getRight(); }
		inline const glm::vec3& getUp() const { return transform.getUp(); }

		virtual const glm::mat4& getViewMatrix() const override { return viewMatrix; }
		virtual const glm::mat4& getProjectionMatrix() const override { return projectionMatrix; }
		virtual const glm::mat4& getViewProjectionMatrix() const override { return viewProjectionMatrix; }

		virtual void setNearPlane(float nearZ) override;
		virtual void setFarPlane(float farZ) override;

		virtual const Ref<UniformBuffer>& getMatrixBuffer() const override { return matrixBuffer; }
	};
#pragma endregion


#pragma region PerspectiveCamera
	class PerspectiveCamera : public Camera
	{
	private:
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::mat4 viewProjectionMatrix;

		float fov, ratio;

		Ref<UniformBuffer> matrixBuffer;

		void recalculateViewMatrix();
		void recalculateProjectionMatrix();

	public:
		PerspectiveCamera(float fov, float ratio, float nearPlane, float farPlane);

		void setPosition(const glm::vec3& pos);
		void setRotation(const glm::quat& rot);

		inline const glm::vec3& getPosition() const { return transform.getPosition(); }
		inline const glm::quat& getRotation() const { return transform.getRotation(); }

		inline const glm::vec3& getForward() const { return transform.getForward(); }
		inline const glm::vec3& getRight() const { return transform.getRight(); }
		inline const glm::vec3& getUp() const { return transform.getUp(); }

		void setFOV(float fov);
		void setRatio(float ratio);

		virtual const glm::mat4& getViewMatrix() const override { return viewMatrix; }
		virtual const glm::mat4& getProjectionMatrix() const override { return projectionMatrix; }
		virtual const glm::mat4& getViewProjectionMatrix() const override { return viewProjectionMatrix; }

		virtual void setNearPlane(float nearZ) override;
		virtual void setFarPlane(float farZ) override;

		virtual const Ref<UniformBuffer>& getMatrixBuffer() const override { return matrixBuffer; }
	};
#pragma endregion
}