#pragma once

#include "RenderPasses/RenderPass.h"

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
		float size = 10.0f;
		float fov = glm::radians(45.0f), ratio = 1.0f;

		std::vector<RenderPass*> renderpasses;

	public:
		Camera();

		// TODO: Use Ref and CreateRef with the different RenderPasses
		Camera(ProjectionType projection, float fov, float nearPlane = 0.03f, float farPlane = 1000.0f,
			const std::initializer_list<RenderPass*>& renderpasses = {});

		Camera(const glm::mat4& projection, const std::initializer_list<RenderPass*>& renderpasses = {})
			: projectionMatrix(projection), nearZ(0), farZ(0), renderpasses(renderpasses) {}

		void RecalculateProjectionMatrix();

		void SetPerspective(float fov, float nearPlane = 0.03f, float farPlane = 1000.0f);

		void SetOrthographic(float size, float nearPlane = 0.03f, float farPlane = 1000.0f);

		void SetRenderPasses(const std::vector<RenderPass*>& renderpasses);

		void SetFov(float f) { fov = f; changed = true; }
		void SetSize(float s) { size = s; changed = true; }
		void SetRatio(float r) { ratio = r; changed = true; }
		void SetNearPlane(float nearPlane) { nearZ = nearPlane; changed = true; }
		void SetFarPlane(float farPlane) { farZ = farPlane; changed = true; }
		void SetProjectionType(ProjectionType type) { projectionType = type; changed = true; }
		void SetProjectionMatrix(const glm::mat4& projection) { projectionMatrix = projection; }

		float GetFov() const { return fov; }
		float GetSize() const { return size; }
		float GetRatio() const { return ratio; }
		float GetNearPlane() const { return nearZ; }
		float GetFarPlane() const { return farZ; }
		const std::vector<RenderPass*>& GetRenderPasses() const { return renderpasses; }
		ProjectionType GetProjectionType() const { return projectionType; }
		const glm::mat4& GetProjectionMatrix() const { return projectionMatrix; }
	};
}