#include "ebpch.h"
#include "SceneHierarchyPanel.h"

#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include <sstream>

namespace Mahakam::Editor
{
	void SceneHierarchyPanel::DrawEntityNode(Entity entity, Ref<Scene> context)
	{
		std::string& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((entity == Selection::GetSelectedEntity()) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

		auto& relation = entity.GetComponent<RelationshipComponent>();

		// Remove leaf node on parents with no children
		if (relation.First == entt::null)
			flags |= ImGuiTreeNodeFlags_Leaf;

		// Create tag
		std::stringstream tagStream;

		// Add icons
		for (auto& [name, componentInterface] : ComponentRegistry::GetComponents())
		{
			if (componentInterface.Icon && componentInterface.HasComponent(entity))
				tagStream << componentInterface.Icon;
		}

		// Use default icon
		if (tagStream.tellp() == 0)
			tagStream << u8"\ueea5"; // Archive icon

		tagStream << " " << tag.c_str();
		std::string tagString = tagStream.str();
		const char* tagName = tagString.c_str();

		bool open = ImGui::TreeNodeEx((void*)(uint64_t)uint32_t(entity), flags, "%s", tagName);

		// If entity is clicked (not the arrow)
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen())
			Selection::SetSelectedEntity(entity);

		// Set as drag source for all it's components
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Entity", &entity, sizeof(Entity));
			ImGui::Text("(%d) %s", uint32_t(entity), tagName);
			ImGui::EndDragDropSource();
		}

		// If entity is right-clicked
		if (ImGui::BeginPopupContextItem())
		{
			char buffer[GUI::MAX_STR_LEN]{ 0 };
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Entity Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}

			ImGui::SameLine();
			ImGui::PushItemWidth(-FLT_MIN);

			// Draw add component popup
			if (ImGui::Button("+"))
				ImGui::OpenPopup("AddComponentEntity");

			if (ImGui::BeginPopup("AddComponentEntity"))
			{
				for (auto& [name, componentInterface] : ComponentRegistry::GetComponents())
				{
					if (!componentInterface.HasComponent(entity))
					{
						if (ImGui::MenuItem(name.c_str()))
						{
							componentInterface.AddComponent(entity);
							ImGui::CloseCurrentPopup();
						}
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopItemWidth();

			if (ImGui::MenuItem("Create empty entity"))
			{
				Entity child = context->CreateEntity();
				child.SetParent(entity);
			}

			if (ImGui::MenuItem("Delete"))
			{
				if (Selection::GetSelectedEntity() == entity)
					Selection::SetSelectedEntity({});

				entity.Delete();
			}

			ImGui::EndPopup();
		}

		// If entity is open
		if (open)
		{
			Entity current = entity.GetFirstChild();
			while (current)
			{
				DrawEntityNode(current, context);

				current = current.GetNext();
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

			char buffer[GUI::MAX_STR_LEN]{ 0 };
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
				std::stringstream tagStream;
				if (componentInterface.Icon)
					tagStream << componentInterface.Icon << " ";
				tagStream << name;

				std::string tagString = tagStream.str();

				bool markedForDeletion = false;
				if (ImGui::CollapsingHeader(tagString.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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
					// context->ForEach<RelationshipComponent>([&](auto handle, RelationshipComponent& relation)
					// {
					// 	// https://skypjack.github.io/2019-08-20-ecs-baf-part-4-insights/
					// 	Entity entity{ handle, context.get() };

					// 	// Start by drawing the root entities
					// 	// Recursively draw their children
					// 	//if (!relation.Parent)
					// 		DrawEntityNode(entity, context);
					// });

					context->ForEachEntity([&](Entity entity)
					{
						// https://skypjack.github.io/2019-08-20-ecs-baf-part-4-insights/
						auto& relation = entity.GetComponent<RelationshipComponent>();

						// Start by drawing the root entities
						// Recursively draw their children
						if (relation.Parent == entt::null)
							DrawEntityNode(entity, context);
					});

					if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
						Selection::SetSelectedEntity({});

					// Blank space menu
					if (ImGui::BeginPopupContextWindow("Hierarchy Empty RMB", 1, false))
					{
						if (ImGui::MenuItem("Create empty entity"))
							context->CreateEntity();

						ImGui::EndPopup();
					}
				}
			}

			ImGui::End();

			if (ImGui::Begin("Inspector"))
			{
				Entity selectedEntity = Selection::GetSelectedEntity();
				if (selectedEntity)
					DrawInspector(selectedEntity);
			}

			ImGui::End();
		}
	}
}