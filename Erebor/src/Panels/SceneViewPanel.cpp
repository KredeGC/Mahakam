#include "ebpch.h"
#include "SceneViewPanel.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include <imguizmo/ImGuizmo.h>

namespace Mahakam::Editor
{
	void SceneViewPanel::OnUpdate(Timestep dt)
	{
		if (m_Open)
		{
			m_EditorCamera.OnUpdate(dt, m_Focused, m_Hovered);

			AudioEngine::UpdateSounds(m_EditorCamera.GetModelMatrix());

			SceneManager::GetActiveScene()->OnRender(m_EditorCamera, m_EditorCamera.GetModelMatrix());
		}
	}

	void SceneViewPanel::OnImGuiRender()
	{
		MH_PROFILE_FUNCTION();

		if (m_Open)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			if (ImGui::Begin("Scene View", &m_Open, ImGuiWindowFlags_MenuBar))
			{
				Camera& camera = m_EditorCamera.GetCamera();
				Camera::ProjectionType projectionType = camera.GetProjectionType();

				ImGui::PopStyleVar();

				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("Camera"))
					{
						if (ImGui::BeginMenu("Projection"))
						{
							if (ImGui::MenuItem("Perspective", nullptr, projectionType == Camera::ProjectionType::Perspective))
								camera.SetProjectionType(Camera::ProjectionType::Perspective);

							if (ImGui::MenuItem("Orthographic", nullptr, projectionType == Camera::ProjectionType::Orthographic))
								camera.SetProjectionType(Camera::ProjectionType::Orthographic);
							
							ImGui::EndMenu();
						}

						ImGui::PushItemWidth(80.0f);

						if (projectionType == Camera::ProjectionType::Perspective)
						{
							float fov = glm::degrees(camera.GetFov());
							if (ImGui::DragFloat("Fov##Editor Camera Fov", &fov, 0.1f, 0.0f, 180.0f))
								camera.SetFov(glm::radians(fov));
						}
						else
						{
							float size = camera.GetSize();
							if (ImGui::DragFloat("Size##Editor Camera Size", &size, 0.1f, 0.0f))
								camera.SetSize(size);
						}

						float nearZ = camera.GetNearPlane();
						if (ImGui::DragFloat("Near Z##Editor Camera NearZ", &nearZ, 0.1f, 0.0f))
							camera.SetNearPlane(nearZ);

						float farZ = camera.GetFarPlane();
						if (ImGui::DragFloat("Far Z##Editor Camera FarZ", &farZ, 0.1f, 0.0f))
							camera.SetFarPlane(farZ);

						ImGui::PopItemWidth();
						
						ImGui::EndMenu();
					}

					ImGui::MenuItem(u8"\ueca6");
					ImGui::MenuItem(u8"\uecb1");
					ImGui::MenuItem(u8"\ueca5");
					
					ImGui::EndMenuBar();
				}

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

				auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
				auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
				auto viewportOffset = ImGui::GetWindowPos();
				m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
				m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

				m_Focused = ImGui::IsWindowFocused();
				m_Hovered = ImGui::IsWindowHovered();
				Application::GetInstance()->GetImGuiLayer()->BlockEvents(!m_Focused && !m_Hovered);
				ImVec2 newViewportSize = ImGui::GetContentRegionAvail();
				if (newViewportSize.x != m_ViewportSize.x || newViewportSize.y != m_ViewportSize.y)
				{
					m_ViewportSize.x = newViewportSize.x;
					m_ViewportSize.y = newViewportSize.y;

					SceneManager::GetActiveScene()->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

					m_EditorCamera.GetCamera().SetRatio(m_ViewportSize.x / m_ViewportSize.y);
				}

				Ref<FrameBuffer> framebuffer = Renderer::GetFrameBuffer();
				if (framebuffer)
				{
					Asset<Texture> viewportTexture = framebuffer->GetColorTexture(0);
					if (viewportTexture)
						ImGui::Image((void*)(uintptr_t)viewportTexture->GetRendererID(), newViewportSize, ImVec2(0, 1), ImVec2(1, 0));
				}

				// Setup ImGuizmo
				ImVec2 pos = { m_ViewportBounds[0].x, m_ViewportBounds[0].y };
				ImVec2 size = { m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y };

				ImGuizmo::SetOrthographic(projectionType == Camera::ProjectionType::Orthographic);
				ImGuizmo::SetDrawlist();

				ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

				// View manipulation
				{
					// FIXME: Gets clamped in the positive z-dome. Possibly due to interference in EditorCamera::OnUpdate
					glm::mat4 viewMatrix = glm::inverse(m_EditorCamera.GetModelMatrix());

					ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), 5.0f, ImVec2(pos.x + size.x - 128, pos.y), ImVec2(128, 128), 0);
				}

				// Transform gizmo
				Entity selectedEntity = Selection::GetSelectedEntity();
				if (selectedEntity && selectedEntity.HasComponent<TransformComponent>() && m_GizmoType != -1)
				{
					const glm::mat4 viewMatrix = glm::inverse(m_EditorCamera.GetModelMatrix());

					ImGuizmo::Enable(!m_EditorCamera.IsControlling());

					// Setup camera matrices
					const glm::mat4& projectionMatrix = m_EditorCamera.GetCamera().GetProjectionMatrix();

					// Setup model matrix
					TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
					glm::mat4 modelMatrix = transform.GetModelMatrix();

					// Setup snapping
					bool snap = Input::IsKeyPressed(Key::LEFT_SHIFT);
					float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;
					float snapValues[3] = { snapValue, snapValue, snapValue };

					// Draw transform gizmo
					ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
						(ImGuizmo::OPERATION)m_GizmoType, (ImGuizmo::MODE)m_LocalScope, glm::value_ptr(modelMatrix), nullptr, snap ? snapValues : 0);

					if (ImGuizmo::IsUsing())
					{
						glm::mat4 parentMatrix{ 1.0f };

						if (selectedEntity.HasComponent<RelationshipComponent>())
						{
							Entity parentEntity = selectedEntity.GetParent();
							if (parentEntity)
							{
								if (TransformComponent* parentTransform = parentEntity.TryGetComponent<TransformComponent>())
									parentMatrix = glm::inverse(parentTransform->GetModelMatrix());
							}
						}

						glm::mat4 localMatrix = parentMatrix * modelMatrix;

						glm::vec3 position, scale;
						glm::quat rotation;

						Math::DecomposeTransform(localMatrix, position, rotation, scale);

						if (m_GizmoType == ImGuizmo::OPERATION::TRANSLATE)
							transform.SetPosition(position);
						else if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
							transform.SetRotation(rotation);
						else if (m_GizmoType == ImGuizmo::OPERATION::SCALE)
							transform.SetScale(scale);
					}
				}
			}

			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	bool SceneViewPanel::OnEvent(Event& event)
	{
		if (m_Hovered)
		{
			EventDispatcher dispatcher(event);
			dispatcher.DispatchEvent<MouseScrolledEvent>(MH_BIND_EVENT(m_EditorCamera.OnMouseScroll));
			dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(m_EditorCamera.OnKeyPressed));
			dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(SceneViewPanel::OnKeyPressed));
		}

		return false;
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
		case Key::Q:
			m_GizmoType = -1;
			break;
		case Key::W:
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case Key::R:
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		case Key::G:
			m_LocalScope = !m_LocalScope;
			break;
		default:
			break;
		}

		return false;
	}
}