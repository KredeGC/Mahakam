#include "mhpch.h"
#include "GUI.h"

namespace Mahakam::GUI
{
	bool DrawVec3Control(const char* label, glm::vec3& value, float resetValue)
	{
		bool changed = false;

		ImGui::PushID(&value);

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

		ImGui::PopID();

		return changed;
	}

	bool DrawColor3Edit(const char* label, glm::vec3& value, ImGuiColorEditFlags flags)
	{
		bool changed = false;

		ImGui::PushID(&value);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, COLUMN_WIDTH);

		ImGui::Text("%s", label);
		ImGui::NextColumn();

		changed = ImGui::ColorEdit3("", glm::value_ptr(value), flags);

		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}

	bool DrawDragDropTarget(const std::string& label, const std::string& extension, std::filesystem::path& importPath)
	{
		static constexpr int MAX_STR_LEN = 256; // TODO: Un-hardcode this

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
		}

		return false;
	}
}