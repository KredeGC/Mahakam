#include "ebpch.h"
#include "EditorCamera.h"

namespace Mahakam
{
	void EditorCamera::OnUpdate(Timestep dt, bool focus, bool hover)
	{
		bool altHeld = Input::IsKeyPressed(MH_KEY_LEFT_ALT);

		float zoomDelta = 0.0f;

		// Calculate mouse delta
		auto [mouseX, mouseY] = Input::GetMousePos();
		float deltaX = mouseX - oldMouseX;
		float deltaY = mouseY - oldMouseY;
		oldMouseX = mouseX;
		oldMouseY = mouseY;

		if (focus && Input::IsKeyPressed(MH_KEY_F)) // Focus on something
		{
			// TODO: Focus on selected entity in hierarchy

			target = { 0.0f, 0.0f, 0.0f };
			zoom = glm::length(target - position);
		}
		else if (focus && altHeld && Input::IsMouseButtonPressed(MH_MOUSE_BUTTON_LEFT)) // Orbit around target
		{
			eulerAngles.y -= glm::radians(deltaX * dragSpeed);
			eulerAngles.x -= glm::radians(deltaY * dragSpeed);

			UpdateRotationMatrix();
		}
		else if (focus && !altHeld && Input::IsMouseButtonPressed(MH_MOUSE_BUTTON_RIGHT)) // FPS Rotate
		{
			// TODO: Lock and hide cursor

			eulerAngles.y -= glm::radians(deltaX * dragSpeed);
			eulerAngles.x -= glm::radians(deltaY * dragSpeed);

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
		else if (focus && altHeld && Input::IsMouseButtonPressed(MH_MOUSE_BUTTON_RIGHT)) // Right zoom
		{
			float speed = GetZoomSpeed();

			zoomDelta -= deltaX * speed;
			zoomDelta += deltaY * speed;
		}
		else if (focus && Input::IsMouseButtonPressed(MH_MOUSE_BUTTON_MIDDLE)) // Panning
		{
			float speed = GetZoomSpeed();

			target += deltaY * speed * up - deltaX * speed * right;
		}

		zoom = glm::max(zoom + zoomDelta, 0.0f);
		position = target - forward * zoom;

		UpdateModelMatrix();
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
		rotationMatrix = glm::mat4(glm::quat(eulerAngles));

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