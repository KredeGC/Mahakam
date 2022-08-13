#include "ebpch.h"
#include "SceneHierarchyPanel.h"

#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace Mahakam::Editor
{
	void SceneHierarchyPanel::DrawEntityNode(Entity entity, Ref<Scene> context)
	{
		if (!m_SafeContext) return;

		std::string& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((entity == Selection::GetSelectedEntity()) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

		auto& relation = entity.GetComponent<RelationshipComponent>();

		if (!relation.First)
			flags |= ImGuiTreeNodeFlags_Leaf;

		char tagName[256];
		strcpy(tagName, u8"\ueea5");
		strcat(tagName, tag.c_str());

		bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", tagName);

		if (ImGui::IsItemClicked())
			Selection::SetSelectedEntity(entity);

		if (ImGui::BeginPopupContextItem())
		{
			std::string label = std::string("Delete ") + std::to_string(entity);
			if (ImGui::MenuItem(label.c_str()))
			{
				m_SafeContext = false;

				if (Selection::GetSelectedEntity() == entity)
					Selection::SetSelectedEntity({});

				context->DestroyEntity(entity);
			}

			ImGui::EndPopup();
		}

		if (open)
		{
			if (m_SafeContext)
			{
				Entity current = relation.First;
				while (current)
				{
					DrawEntityNode(current, context);

					if (!m_SafeContext)
					{
						ImGui::TreePop();
						return;
					}

					current = current.GetComponent<RelationshipComponent>().Next;
				}
			}

			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawInspector(Entity entity)
	{
		// Draw tag as the first component
		if (entity.HasComponent<TagComponent>())
		{
			std::string& tag = entity.GetComponent<TagComponent>().Tag;

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
						componentInterface.AddComponent(Selection::GetSelectedEntity());
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

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

					PropertyRegistry::PropertyPtr onInspector = PropertyRegistry::GetProperty(name);
					if (onInspector)
						onInspector(entity);
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
			if (ImGui::Begin("Scene Hierarchy", &m_Open))
			{
				Ref<Scene> context = SceneManager::GetActiveScene();
				if (context)
				{
					std::vector<Entity> entities;

					m_SafeContext = true;
					context->ForEachEntity([&](auto handle)
					{
						if (m_SafeContext)
						{
							Entity entity(handle, context.get());

							auto& relation = entity.GetComponent<RelationshipComponent>();

							if (!relation.Parent)
								DrawEntityNode(entity, context);
						}
					});


					context->ForEachEntity([&](auto handle)
					{
						Entity entity(handle, context.get());

						// TODO: https://skypjack.github.io/2019-08-20-ecs-baf-part-4-insights/
						// In short: Sort by the relationship between parent and child
						// TODO: Find a way for the user to change the order?
					});



					/*context->ForEachEntity([&](auto handle)
					{
						Entity entity(handle, context.get());

						DrawEntityNode(entity, context);
					});*/

					if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
						Selection::SetSelectedEntity({});

					// Blank space menu
					if (ImGui::BeginPopupContextWindow(0, 1, false))
					{
						if (ImGui::MenuItem("Create empty entity"))
							context->CreateEntity();

						ImGui::EndPopup();
					}

					ImGui::End();


					ImGui::Begin("Inspector");

					if (Selection::GetSelectedEntity())
					{
						DrawInspector(Selection::GetSelectedEntity());
					}
				}
			}

			ImGui::End();
		}
	}
}