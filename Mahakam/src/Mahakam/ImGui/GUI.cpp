#include "Mahakam/mhpch.h"
#include "GUI.h"

#ifndef MH_STANDALONE
#include "Mahakam/Editor/Selection.h"
#endif

#include "Mahakam/Core/Input.h"

#include "Mahakam/Scene/ComponentRegistry.h"
#include "Mahakam/Scene/Components/TagComponent.h"

#include <imgui/imgui_internal.h>

namespace Mahakam::GUI
{
	bool DrawVec3Control(const char* label, glm::vec3& value, float resetValue, bool disabled)
	{
		bool changed = false;

		ImGui::PushID(&value);
		ImGui::BeginDisabled(disabled);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_WIDTH);

		ImGui::Text("%s", label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize(lineHeight + 3.0f, lineHeight);


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
		if (ImGui::Button("X", buttonSize))
		{
			changed = true;
			value.x = resetValue;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &value.x, 0.1f))
			changed = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
		if (ImGui::Button("Y", buttonSize))
		{
			changed = true;
			value.y = resetValue;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &value.y, 0.1f))
			changed = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
		if (ImGui::Button("Z", buttonSize))
		{
			changed = true;
			value.z = resetValue;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &value.z, 0.1f))
			changed = true;
		ImGui::PopItemWidth();


		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::EndDisabled();
		ImGui::PopID();

		return changed;
	}

	bool DrawColor3Edit(const char* label, glm::vec3& value, ImGuiColorEditFlags flags)
	{
		bool changed = false;

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_WIDTH);

		ImGui::Text("%s", label);
		ImGui::NextColumn();

		ImGui::PushID(label);

		changed = ImGui::ColorEdit3("", glm::value_ptr(value), flags);

		ImGui::PopID();

		ImGui::Columns(1);

		return changed;
	}

	bool DrawDragDropEntity(const std::string& label, const std::string& component, Entity& entity)
	{
		std::string entityLabel = "undefined";

		if (entity && entity.IsValid())
		{
			if (TagComponent* tag = entity.TryGetComponent<TagComponent>())
				entityLabel = tag->Tag;
			else
				entityLabel = std::to_string(uint32_t(entity));
		}

		ImGui::Button(entityLabel.c_str());

		// Select entity on double click
#ifndef MH_STANDALONE
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			Editor::Selection::SetSelectedEntity(entity);
#endif

		// Remove entity reference
		if ((ImGui::IsItemFocused() && Input::IsKeyPressed(Key::Delete))
			|| (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
			entity = {};

		// Set as drop target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity", ImGuiDragDropFlags_AcceptPeekOnly))
			{
				Entity dragEntity = *((Entity*)payload->Data);

				auto& components = ComponentRegistry::GetComponents();
				auto iter = components.find(component);
				if (iter != components.end())
				{
					if (iter->second.HasComponent(dragEntity))
					{
						ImGui::AcceptDragDropPayload("Entity");

						if (payload->IsDelivery())
						{
							entity = dragEntity;
							return true;
						}
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		ImGui::Text("%s", label.c_str());

		return false;
	}

	bool DrawDragDropEntity(const std::string& label, Entity& entity)
	{
		std::string entityLabel = "undefined";

		if (entity && entity.IsValid())
		{
			if (TagComponent* tag = entity.TryGetComponent<TagComponent>())
				entityLabel = tag->Tag;
			else
				entityLabel = std::to_string(uint32_t(entity));
		}

		ImGui::Button(entityLabel.c_str());

		// Select entity on double click
#ifndef MH_STANDALONE
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			Editor::Selection::SetSelectedEntity(entity);
#endif

		// Remove entity reference
		if ((ImGui::IsItemFocused() && Input::IsKeyPressed(Key::Delete))
			|| (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
			entity = {};

		// Set as drop target
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
			{
				entity = *((Entity*)payload->Data);
				return true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		ImGui::Text("%s", label.c_str());

		return false;
	}

	bool DrawDragDropField(const std::string& label, const std::string& extension, std::filesystem::path& importPath)
	{
		std::string importString = importPath.string();
		char filepathBuffer[MAX_STR_LEN]{ 0 };
		strncpy(filepathBuffer, importString.c_str(), importString.size());
		if (ImGui::InputText(label.c_str(), filepathBuffer, MAX_STR_LEN))
		{
			importPath = std::string(filepathBuffer);

			return true;
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(extension.c_str()))
			{
				importPath = (const char*)payload->Data;
				return true;
			}

			ImGui::EndDragDropTarget();
		}

		return false;
	}

	bool DrawDragDropTarget(const std::vector<std::string>& extensions, std::filesystem::path& importPath)
	{
		if (ImGui::BeginDragDropTarget())
		{
			for (auto& extension : extensions)
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(extension.c_str()))
				{
					importPath = (const char*)payload->Data;
					return true;
				}
			}

			ImGui::EndDragDropTarget();
		}

		return false;
	}

	bool DrawColor3Edit(const std::string& label, glm::vec3& value, ImGuiColorEditFlags flags)
	{
		return ImGui::ColorEdit3(label.c_str(), glm::value_ptr(value), flags);
	}

	bool DrawColor4Edit(const std::string& label, glm::vec4& value, ImGuiColorEditFlags flags)
	{
		return ImGui::ColorEdit4(label.c_str(), glm::value_ptr(value), flags);
	}

	bool DrawFloatSlider(const std::string& label, float& value, float min, float max)
	{
		return ImGui::SliderFloat(label.c_str(), &value, min, max);
	}

	bool DrawFloat2Slider(const std::string& label, glm::vec2& value, float min, float max)
	{
		return ImGui::SliderFloat2(label.c_str(), glm::value_ptr(value), min, max);
	}

	bool DrawFloat3Slider(const std::string& label, glm::vec3& value, float min, float max)
	{
		return ImGui::SliderFloat3(label.c_str(), glm::value_ptr(value), min, max);
	}

	bool DrawFloat4Slider(const std::string& label, glm::vec4& value, float min, float max)
	{
		return ImGui::SliderFloat4(label.c_str(), glm::value_ptr(value), min, max);
	}

	bool DrawFloatDrag(const std::string& label, float& value, float speed, float min, float max)
	{
		return ImGui::DragFloat(label.c_str(), &value, speed, min, max);
	}

	bool DrawFloat2Drag(const std::string& label, glm::vec2& value, float speed, float min, float max)
	{
		return ImGui::DragFloat2(label.c_str(), glm::value_ptr(value), speed, min, max);
	}

	bool DrawFloat3Drag(const std::string& label, glm::vec3& value, float speed, float min, float max)
	{
		return ImGui::DragFloat3(label.c_str(), glm::value_ptr(value), speed, min, max);
	}

	bool DrawFloat4Drag(const std::string& label, glm::vec4& value, float speed, float min, float max)
	{
		return ImGui::DragFloat4(label.c_str(), glm::value_ptr(value), speed, min, max);
	}

	bool DrawIntDrag(const std::string& label, int32_t& value, float speed, int32_t min, int32_t max)
	{
		return ImGui::DragInt(label.c_str(), &value, speed, min, max);
	}
}