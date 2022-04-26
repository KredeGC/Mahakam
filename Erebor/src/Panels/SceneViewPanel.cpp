#include "ebpch.h"
#include "SceneViewPanel.h"

#include "EditorLayer.h"

#include <glm/gtc/type_ptr.hpp>

#include <ImGuizmo.h>

namespace Mahakam::Editor
{
	void SceneViewPanel::OnUpdate(Timestep dt)
	{
		if (m_Open)
			m_EditorCamera.OnUpdate(dt, m_Focused, m_Hovered);
	}

	void SceneViewPanel::OnImGuiRender()
	{
		MH_PROFILE_FUNCTION();

		if (m_Open)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Scene View", &m_Open);

			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			m_Focused = ImGui::IsWindowFocused();
			m_Hovered = ImGui::IsWindowHovered();
			Application::GetInstance().GetImGuiLayer()->BlockEvents(!m_Focused && !m_Hovered);
			ImVec2 newViewportSize = ImGui::GetContentRegionAvail();
			if (newViewportSize.x != m_ViewportSize.x || newViewportSize.y != m_ViewportSize.y)
			{
				m_ViewportSize.x = newViewportSize.x;
				m_ViewportSize.y = newViewportSize.y;

				m_EditorCamera.GetCamera().SetRatio(m_ViewportSize.x / m_ViewportSize.y);
			}

			if (m_ViewportTexture)
				ImGui::Image((void*)(uintptr_t)m_ViewportTexture->GetRendererID(), newViewportSize, ImVec2(0, 1), ImVec2(1, 0));

			// Setup ImGuizmo
			ImVec2 pos = { m_ViewportBounds[0].x, m_ViewportBounds[0].y };
			ImVec2 size = { m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y };

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

			// View manipulation
			{
				// FIXME: Gets clamped in the positive z-dome. Possibly due to interference in EditorCamera::OnUpdate
				glm::mat4 viewMatrix = glm::inverse(m_EditorCamera.GetModelMatrix());

				ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), 5.0f, ImVec2(pos.x + size.x - 128, pos.y), ImVec2(128, 128), 0);

				/*const glm::mat4 modelMatrix = glm::inverse(viewMatrix);

				glm::quat rotation = glm::quat_cast(modelMatrix);

				editorCamera.SetRotation(rotation);*/

				//MH_CORE_TRACE("{0},{1},{2}", viewEulerAngles.x - viewEulerAngles2.x, viewEulerAngles.y - viewEulerAngles2.y, viewEulerAngles.z - viewEulerAngles2.z);
			}

			// Transform gizmo
			Entity selectedEntity = EditorLayer::GetSelectedEntity();
			if (selectedEntity && m_GizmoType != -1)
			{
				const glm::mat4 viewMatrix = glm::inverse(m_EditorCamera.GetModelMatrix());

				ImGuizmo::Enable(!m_EditorCamera.IsControlling());

				// Setup camera matrices
				const glm::mat4& projectionMatrix = m_EditorCamera.GetCamera().GetProjectionMatrix();

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
		if (m_Hovered)
		{
			EventDispatcher dispatcher(event);
			dispatcher.DispatchEvent<MouseScrolledEvent>(MH_BIND_EVENT(m_EditorCamera.OnMouseScroll));
			dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(m_EditorCamera.OnKeyPressed));
			dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(SceneViewPanel::OnKeyPressed));
		}
	}

	bool SceneViewPanel::OnKeyPressed(KeyPressedEvent& event)
	{
		if (m_EditorCamera.IsControlling())
			return false;

		if (event.GetRepeatCount() > 0)
			return false;

		ImGuiIO& io = ImGui::GetIO();

		if (io.WantCaptureKeyboard)
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