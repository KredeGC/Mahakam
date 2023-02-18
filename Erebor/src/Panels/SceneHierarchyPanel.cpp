#include "ebpch.h"
#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>
#include <sstream>

namespace Mahakam::Editor
{
	static std::string CreateTagIcons(Entity entity, const std::string& tag)
	{
		// Create stream for icons
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

		// Add tag at the end
		tagStream << " " << tag.c_str();
		return tagStream.str();
	}

	template<typename Fn>
	static void DrawDropTarget(Ref<Scene> context, Entity entity, Fn func)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
			{
				Entity move = *((Entity*)payload->Data);

				if (entity != move)
				{
					// Make sure that circular parenting never happens
					Entity current = entity;
					while (current)
					{
						if (current == move)
							return;

						current = current.GetParent();
					}

					func(entity, move);
					context->Sort();
				}
			}

			ImGui::EndDragDropTarget();
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity, Ref<Scene> context)
	{
		std::string& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((entity == Selection::GetSelectedEntity()) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

		auto& relation = entity.GetComponent<RelationshipComponent>();

		// Remove leaf node on parents with no children
		if (relation.First == entt::null)
			flags |= ImGuiTreeNodeFlags_Leaf;

		// Create tag
		std::string tagString = CreateTagIcons(entity, tag);
		const char* tagName = tagString.c_str();

		bool open = ImGui::TreeNodeEx((void*)(uint64_t)entity.GetHandle(), flags, "%s", tagName);

		// If entity is clicked (not the arrow)
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen())
			Selection::SetSelectedEntity(entity);

		// Set as drag source for all it's components
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Entity", &entity, sizeof(Entity));
			ImGui::Text("%s", tagName);
			ImGui::EndDragDropSource();
		}

		// If entity is right-clicked
		if (ImGui::BeginPopupContextItem())
		{
			// Draw entity tag name
			ImGui::PushItemWidth(ImGui::CalcTextSize(tag.c_str()).x + 18.f);

			char buffer[GUI::MAX_STR_LEN]{ 0 };
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Entity Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}

			ImGui::PopItemWidth();

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

		// Mark as drop target
		DrawDropTarget(context, entity, [](Entity parent, Entity move)
		{
			move.SetParent(parent);
		});

		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();

		// If entity is open
		if (open)
		{
			// Draw separator before children
			if (relation.First != entt::null)
			{
				float separatorWidth = ImGui::GetContentRegionAvail().x;

				/*if (ImGui::GetDragDropPayload())
					ImGui::Button("##Visible Separator", { separatorWidth, SEPARATOR_HEIGHT });
				else
					*/
				ImGui::InvisibleButton(tagName, { separatorWidth, SEPARATOR_HEIGHT });

				DrawDropTarget(context, entity, [](Entity parent, Entity move)
				{
					parent.SetFirstChild(move);
				});
			}

			// Draw child entities
			Entity current = entity.GetFirstChild();
			while (current)
			{
				DrawEntityNode(current, context);

				current = current.GetNext();
			}

			ImGui::TreePop();
		}

		// Draw last separator if at root, not open or has no children
		if (!open || relation.First == entt::null)
		{
			float separatorWidth = ImGui::GetContentRegionAvail().x;

			/*if (ImGui::GetDragDropPayload())
				ImGui::Button("##Visible Separator", { separatorWidth, SEPARATOR_HEIGHT });
			else
				*/
			ImGui::InvisibleButton(tagName, { separatorWidth, SEPARATOR_HEIGHT });

			DrawDropTarget(context, entity, [](Entity parent, Entity move)
			{
				parent.SetNext(move);
			});
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

				// Draw header for component
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

					auto onPropertyDraw = componentInterface.OnPropertyDraw;
					if (onPropertyDraw && !markedForDeletion)
					{
						ImGui::Indent();
						onPropertyDraw(entity);
						ImGui::Unindent();
					}
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
					// Draw each entity in the hierarchy
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
					context->ForEachEntity([&](Entity entity)
					{
						// https://skypjack.github.io/2019-08-20-ecs-baf-part-4-insights/
						auto& relation = entity.GetComponent<RelationshipComponent>();

						// Start by drawing the root entities
						// Recursively draw their children
						if (relation.Parent == entt::null)
							DrawEntityNode(entity, context);
					});

					// Use the remaining space as drop target
					if (ImGui::GetDragDropPayload())
					{
						ImVec2 size = ImGui::GetContentRegionAvail();

						if (size.y > 0.0f)
						{
							ImGui::InvisibleButton("##Empty Hierarchy", { size.x, size.y });

							DrawDropTarget(context, {}, [](Entity parent, Entity move)
							{
								move.SetParent({});
							});
						}
					}

					ImGui::PopStyleVar();

					if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
						Selection::SetSelectedEntity({});

					// Blank space menu
					if (ImGui::BeginPopupContextWindow("Hierarchy Empty RMB", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
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