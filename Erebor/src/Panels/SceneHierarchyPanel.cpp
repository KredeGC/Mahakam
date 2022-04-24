#include "ebpch.h"
#include "SceneHierarchyPanel.h"

#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace Mahakam
{
	void SceneHierarchyPanel::DrawEntityNode(Entity entity, Ref<Scene> context)
	{
		std::string& tag = entity.GetComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((entity == EditorLayer::GetSelectedEntity()) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
			EditorLayer::SetSelectedEntity(entity);

		bool markedAsDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				markedAsDeleted = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			ImGui::TreePop();
		}

		if (markedAsDeleted)
		{
			if (EditorLayer::GetSelectedEntity() == entity)
				EditorLayer::SetSelectedEntity({});

			context->DestroyEntity(entity);
		}
	}

	void SceneHierarchyPanel::DrawInspector(Entity entity)
	{
		// Draw tag as the first component
		if (entity.HasComponent<TagComponent>())
		{
			std::string& tag = entity.GetComponent<TagComponent>().tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);


		// Draw add component popup
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			for (auto& [name, componentInterface] : ComponentRegistry::GetComponents())
			{
				if (!componentInterface.HasComponent(entity))
				{
					if (ImGui::MenuItem(name.c_str()))
					{
						componentInterface.AddComponent(EditorLayer::GetSelectedEntity());
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		// Draw Transform
		if (entity.HasComponent<TransformComponent>())
		{
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
					ImGui::OpenPopup("Transform");

				if (ImGui::BeginPopup("Transform"))
				{
					if (ImGui::MenuItem("Reset"))
					{
						entity.RemoveComponent<TransformComponent>();
						entity.AddComponent<TransformComponent>();
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				TransformComponent& transform = entity.GetComponent<TransformComponent>();

				glm::vec3 pos = transform.GetPosition();
				if (GUI::DrawVec3Control("Position", pos))
					transform.SetPosition(pos);

				glm::vec3 eulerAngles = glm::degrees(transform.GetEulerAngles());
				if (GUI::DrawVec3Control("Rotation", eulerAngles))
					transform.SetEulerangles(glm::radians(eulerAngles));

				glm::vec3 scale = transform.GetScale();
				if (GUI::DrawVec3Control("Scale", scale, 1.0f))
					transform.SetScale(scale);
			}
		}

		// Draw the rest of the components
		for (auto& [name, componentInterface] : ComponentRegistry::GetComponents())
		{
			if (componentInterface.HasComponent(entity))
			{
				bool markedForDeletion = false;
				if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
						ImGui::OpenPopup(name.c_str());

					if (ImGui::BeginPopup(name.c_str()))
					{
						if (ImGui::MenuItem("Delete Component"))
						{
							markedForDeletion = true;
							ImGui::CloseCurrentPopup();
						}

						if (ImGui::MenuItem("Reset"))
						{
							componentInterface.RemoveComponent(entity);
							componentInterface.AddComponent(entity);
							ImGui::CloseCurrentPopup();
						}

						ImGui::EndPopup();
					}

					componentInterface.OnInspector(entity);
				}

				if (markedForDeletion)
					componentInterface.RemoveComponent(entity);
			}
		}
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		if (m_Open)
		{
			ImGui::Begin("Scene Hierarchy", &m_Open);

			Ref<Scene> context = EditorLayer::GetActiveScene();
			if (context)
			{
				context->registry.each([&](auto handle)
				{
					Entity entity(handle, context.get());

					DrawEntityNode(entity, context);
				});

				if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
					EditorLayer::SetSelectedEntity({});

				// Blank space menu
				if (ImGui::BeginPopupContextWindow(0, 1, false))
				{
					if (ImGui::MenuItem("Create empty entity"))
						context->CreateEntity();

					ImGui::EndPopup();
				}

				ImGui::End();


				ImGui::Begin("Inspector");

				if (EditorLayer::GetSelectedEntity())
				{
					DrawInspector(EditorLayer::GetSelectedEntity());
				}
			}

			ImGui::End();
		}
	}
}