#pragma once

#include <Mahakam.h>

namespace Mahakam::Editor
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

		bool controlling = false;
		bool hovered = false;

		float oldMouseX = 0.0f, oldMouseY = 0.0f;

		glm::vec3 position = { 4.5f, 4.5f, 12.5f };
		glm::vec3 eulerAngles = { 0.0f, 0.0f, 0.0f };
		glm::quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };

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

		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnMouseScroll(MouseScrolledEvent& event);

		operator Camera& () { return camera; }
		operator const Camera& () const { return camera; }

		inline bool IsControlling() const { return controlling; }

		inline void SetPosition(const glm::vec3& pos) { position = pos; }

		inline const glm::vec3& GetEulerAngles() const { return eulerAngles; }
		inline void SetEulerAngles(const glm::vec3& angles) { eulerAngles = angles; rotation = glm::quat(eulerAngles); UpdateRotationMatrix(); UpdateModelMatrix(); }

		inline const glm::quat& GetRotation() const { return rotation; }
		inline void SetRotation(const glm::quat& rot) { rotation = rot; eulerAngles = glm::eulerAngles(rotation); UpdateRotationMatrix(); UpdateModelMatrix(); }

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