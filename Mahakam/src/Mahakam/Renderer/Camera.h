#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/trigonometric.hpp>

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
		glm::mat4 m_ProjectionMatrix;

		bool m_Changed = false;

		ProjectionType m_ProjectionType = ProjectionType::Perspective;
		float m_NearZ, m_FarZ;
		float m_Size = 10.0f;
		float m_Fov = glm::radians(45.0f), m_Ratio = 1.0f;

	public:
		Camera();

		Camera(ProjectionType projection, float fov, float nearPlane = 0.03f, float farPlane = 1000.0f);

		Camera(const glm::mat4& projection)
			: m_ProjectionMatrix(projection), m_NearZ(0), m_FarZ(0) {}

		void RecalculateProjectionMatrix();

		void SetPerspective(float fov, float nearPlane = 0.03f, float farPlane = 1000.0f);

		void SetOrthographic(float size, float nearPlane = 0.03f, float farPlane = 1000.0f);

		void SetFov(float f) { m_Fov = f; m_Changed = true; }
		void SetSize(float s) { m_Size = s; m_Changed = true; }
		void SetRatio(float r) { m_Ratio = r; m_Changed = true; }
		void SetNearPlane(float nearPlane) { m_NearZ = nearPlane; m_Changed = true; }
		void SetFarPlane(float farPlane) { m_FarZ = farPlane; m_Changed = true; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; m_Changed = true; }
		void SetProjectionMatrix(const glm::mat4& projection) { m_ProjectionMatrix = projection; }

		float GetFov() const { return m_Fov; }
		float GetSize() const { return m_Size; }
		float GetRatio() const { return m_Ratio; }
		float GetNearPlane() const { return m_NearZ; }
		float GetFarPlane() const { return m_FarZ; }
		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	};
}