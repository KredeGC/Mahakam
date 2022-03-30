#pragma once

#include <Mahakam.h>

namespace Mahakam
{
	class EditorCamera
	{
	private:
		static constexpr float dragSpeed = 0.1f;
		static constexpr float moveSpeed = 10.0f;
		static constexpr float panSpeed = 0.01f;
		static constexpr float zoomSpeed = 0.003f;
		static constexpr float scrollSpeed = 5.0f;

		Camera camera;

		float oldMouseX = 0.0f, oldMouseY = 0.0f;

		glm::vec3 position = { 4.5f, 4.5f, 12.5f };
		glm::vec3 eulerAngles = { 0.0f, 0.0f, 0.0f };

		glm::vec3 forward = { 0.0f, 0.0f, 0.0f };
		glm::vec3 right = { 0.0f, 0.0f, 0.0f };
		glm::vec3 up = { 0.0f, 0.0f, 0.0f };

		glm::vec3 target = { 0.0f, 0.0f, 0.0f };
		float zoom = 0.0f;

		glm::mat4 rotationMatrix;
		glm::mat4 modelMatrix;

	public:
		EditorCamera()
			: camera(Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.03f, 1000.0f)
		{
			UpdateRotationMatrix();

			UpdateModelMatrix();
		}

		void OnUpdate(Timestep dt, bool focus, bool hover);

		bool OnMouseScroll(MouseScrolledEvent& event);

		operator Camera& () { return camera; }
		operator const Camera& () const { return camera; }

		inline void SetTarget(const glm::vec3& orbit) { target = orbit; }
		inline const glm::vec3& GetTarget() const { return target; }
		
		inline Camera& GetCamera() { return camera; }
		inline const glm::mat4& GetModelMatrix() const { return modelMatrix; }

	private:
		float GetZoomSpeed() const;

		void UpdateRotationMatrix();

		void UpdateModelMatrix();
	};
}