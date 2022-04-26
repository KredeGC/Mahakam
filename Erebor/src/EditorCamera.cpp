#include "ebpch.h"
#include "EditorCamera.h"

#include "EditorLayer.h"

namespace Mahakam::Editor
{
	void EditorCamera::OnUpdate(Timestep dt, bool focus, bool hover)
	{
		hovered = hover;

		ImGuiIO& io = ImGui::GetIO();

		bool noCapture = !io.WantCaptureKeyboard;
		bool altHeld = Input::IsKeyPressed(MH_KEY_LEFT_ALT);

		float zoomDelta = 0.0f;

		// Calculate mouse delta
		auto [mouseX, mouseY] = Input::GetMousePos();
		float deltaX = mouseX - oldMouseX;
		float deltaY = mouseY - oldMouseY;
		oldMouseX = mouseX;
		oldMouseY = mouseY;

		if (focus && altHeld && Input::IsMouseButtonPressed(MH_MOUSE_BUTTON_LEFT)) // Orbit around target
		{
			controlling = true;
			Application::GetInstance().GetWindow().SetCursorVisible(false);

			eulerAngles.y -= glm::radians(deltaX * dragSpeed);
			eulerAngles.x -= glm::radians(deltaY * dragSpeed);

			rotation = glm::quat(eulerAngles);

			UpdateRotationMatrix();
		}
		else if (noCapture && hover && !altHeld && Input::IsMouseButtonPressed(MH_MOUSE_BUTTON_RIGHT)) // FPS Rotate
		{
			controlling = true;
			Application::GetInstance().GetWindow().SetCursorVisible(false);

			eulerAngles.y -= glm::radians(deltaX * dragSpeed);
			eulerAngles.x -= glm::radians(deltaY * dragSpeed);

			rotation = glm::quat(eulerAngles);

			UpdateRotationMatrix();

			target = position + forward * zoom;

			float speed = Input::IsKeyPressed(MH_KEY_LEFT_SHIFT) ? moveSpeed * 0.1f : moveSpeed;

			// Movement
			if (Input::IsKeyPressed(MH_KEY_A))
				target -= speed * dt * right;
			else if (Input::IsKeyPressed(MH_KEY_D))
				target += speed * dt * right;

			if (Input::IsKeyPressed(MH_KEY_W))
				target += speed * dt * forward;
			else if (Input::IsKeyPressed(MH_KEY_S))
				target -= speed * dt * forward;

			if (Input::IsKeyPressed(MH_KEY_Q))
				target -= speed * dt * up;
			else if (Input::IsKeyPressed(MH_KEY_E))
				target += speed * dt * up;
		}
		else if (noCapture && hover && altHeld && Input::IsMouseButtonPressed(MH_MOUSE_BUTTON_RIGHT)) // Right zoom
		{
			float speed = GetZoomSpeed();

			zoomDelta -= deltaX * speed;
			zoomDelta += deltaY * speed;
		}
		else if (noCapture && hover && Input::IsMouseButtonPressed(MH_MOUSE_BUTTON_MIDDLE)) // Panning
		{
			float speed = GetZoomSpeed();

			target += deltaY * speed * up - deltaX * speed * right;
		}
		else if (!Application::GetInstance().GetWindow().IsCursorVisible())
		{
			controlling = false;
			Application::GetInstance().GetWindow().SetCursorVisible(true);
		}

		zoom = glm::max(zoom + zoomDelta, 0.0f);
		position = target - forward * zoom;

		UpdateModelMatrix();
	}

	bool EditorCamera::OnKeyPressed(KeyPressedEvent& event)
	{
		if (!hovered || event.GetRepeatCount() > 0)
			return false;

		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureKeyboard)
			return false;

		switch (event.GetKeyCode())
		{
		case MH_KEY_F:
			Entity selectedEntity = EditorLayer::GetSelectedEntity();

			if (selectedEntity)
			{
				TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();

				target = transform.GetPosition();
				zoom = glm::length(target - position);
			}
			break;
		}

		return false;
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& event)
	{
		float delta = event.GetY() * scrollSpeed;

		zoom -= delta;

		position = target - forward * zoom;

		UpdateModelMatrix();

		return false;
	}

	float EditorCamera::GetZoomSpeed() const
	{
		float distance = zoom * 0.2f;
		distance = glm::max(distance, 0.3f);
		float speed = distance * distance;
		speed = glm::min(speed, 10.0f); // Max speed 
		return speed * zoomSpeed;
	}

	void EditorCamera::UpdateRotationMatrix()
	{
		rotationMatrix = glm::mat4(rotation);

		forward = rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
		right = rotationMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		up = rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	}

	void EditorCamera::UpdateModelMatrix()
	{
		modelMatrix = glm::translate(glm::mat4(1.0f), position)
			* rotationMatrix;
	}
}