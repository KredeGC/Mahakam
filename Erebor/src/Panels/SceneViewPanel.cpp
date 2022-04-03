#include "ebpch.h"
#include "SceneViewPanel.h"

#include "EditorLayer.h"

#include <glm/gtc/type_ptr.hpp>

#include <ImGuizmo.h>

namespace Mahakam
{
	void SceneViewPanel::OnUpdate(Timestep dt)
	{
		if (open)
			editorCamera.OnUpdate(dt, focused, hovered);
	}

	void SceneViewPanel::OnImGuiRender()
	{
		MH_PROFILE_FUNCTION();

		if (open)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Scene View", &open);
			focused = ImGui::IsWindowFocused();
			hovered = ImGui::IsWindowHovered();
			Application::GetInstance().GetImGuiLayer()->BlockEvents(!focused && !hovered);
			ImVec2 size = ImGui::GetContentRegionAvail();
			if (size.x != viewportSize.x || size.y != viewportSize.y)
			{
				viewportSize.x = size.x;
				viewportSize.y = size.y;

				editorCamera.GetCamera().SetRatio(viewportSize.x / viewportSize.y);
			}
			if (viewportTexture)
				ImGui::Image((void*)(uintptr_t)viewportTexture->GetRendererID(), size, ImVec2(0, 1), ImVec2(1, 0));

			// Gizmos
			Entity selectedEntity = EditorLayer::GetSelectedEntity();
			if (selectedEntity && m_GizmoType != -1)
			{
				// Setup ImGuizmo
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImVec2 pos = ImGui::GetWindowPos();
				ImVec2 size = ImGui::GetWindowSize();
				ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

				// Setup camera matrices
				const glm::mat4& viewMatrix = glm::inverse(editorCamera.GetModelMatrix());
				const glm::mat4& projectionMatrix = editorCamera.GetCamera().GetProjectionMatrix();

				// Setup model matrix
				TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 modelMatrix = transform.GetModelMatrix();

				// Setup snapping
				bool snap = Input::IsKeyPressed(MH_KEY_LEFT_SHIFT);
				float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;
				float snapValues[3] = { snapValue, snapValue, snapValue };

				// Draw transform gizmo
				ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
					(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::MODE::LOCAL, glm::value_ptr(modelMatrix), 0, snap ? snapValues : 0);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 position, eulerAngles, scale;
					Math::DecomposeTransform(modelMatrix, position, eulerAngles, scale);

					transform.SetPosition(position);
					transform.SetEulerangles(eulerAngles);
					transform.SetScale(scale);
				}
			}


			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	void SceneViewPanel::OnEvent(Event& event)
	{
		if (hovered)
		{
			EventDispatcher dispatcher(event);
			dispatcher.DispatchEvent<MouseScrolledEvent>(MH_BIND_EVENT(editorCamera.OnMouseScroll));
			dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(SceneViewPanel::OnKeyPressed));
		}
	}

	bool SceneViewPanel::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.GetRepeatCount() > 0)
			return false;

		switch (event.GetKeyCode())
		{
		case MH_KEY_Q:
			m_GizmoType = -1;
			break;
		case MH_KEY_W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case MH_KEY_E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case MH_KEY_R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}

		return false;
	}
}