#pragma once

#include <imgui/imgui.h>

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

#include <array>
#include <string>

namespace Mahakam
{
	class Entity;
}

namespace Mahakam::GUI
{
	constexpr float COLUMN_WIDTH = 100.0f;
	constexpr int MAX_STR_LEN = 256;

	bool DrawVec3Control(const char* label, glm::vec3& value, float resetValue = 0.0f, bool disabled = false);

	bool DrawColor3Edit(const char* label, glm::vec3& value, ImGuiColorEditFlags flags = 0);

	bool DrawDragDropEntity(const std::string& label, const std::string& component, Entity& entity);
	bool DrawDragDropEntity(const std::string& label, Entity& entity);

	bool DrawDragDropField(const std::string& label, const std::string& extension, std::filesystem::path& importPath);
	bool DrawDragDropTarget(const std::vector<std::string>& extensions, std::filesystem::path& importPath);

	bool DrawColor3Edit(const std::string& label, glm::vec3& value, ImGuiColorEditFlags flags = ImGuiColorEditFlags_None);
	bool DrawColor4Edit(const std::string& label, glm::vec4& value, ImGuiColorEditFlags flags = ImGuiColorEditFlags_None);

	bool DrawFloatSlider(const std::string& label, float& value, float min, float max);
	bool DrawFloat2Slider(const std::string& label, glm::vec2& value, float min, float max);
	bool DrawFloat3Slider(const std::string& label, glm::vec3& value, float min, float max);
	bool DrawFloat4Slider(const std::string& label, glm::vec4& value, float min, float max);

	bool DrawFloatDrag(const std::string& label, float& value, float speed, float min, float max);
	bool DrawFloat2Drag(const std::string& label, glm::vec2& value, float speed, float min, float max);
	bool DrawFloat3Drag(const std::string& label, glm::vec3& value, float speed, float min, float max);
	bool DrawFloat4Drag(const std::string& label, glm::vec4& value, float speed, float min, float max);

	bool DrawIntDrag(const std::string& label, int32_t& value, float speed, int32_t min, int32_t max);

	template<typename T, size_t Size>
	bool DrawComboBox(const std::string& label, T& value, const char* (&values)[Size])
	{
		bool modified = false;
		const char* valueName = values[value];

		if (ImGui::BeginCombo(label.c_str(), valueName))
		{
			for (T i = 0; i < Size; i++)
			{
				bool selected = value == i;

				if (ImGui::Selectable(values[i], selected))
				{
					value = i;
					modified = true;
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return modified;
	}

	template<typename T, size_t Size>
	bool DrawComboBox(const std::string& label, T& value, const std::array<std::string_view, Size>& values)
	{
		bool modified = false;
		std::string_view valueName;

		if constexpr (std::is_enum_v<T>)
			valueName = values[std::underlying_type_t<T>(value)];
		else
			valueName = values[value];

		if (ImGui::BeginCombo(label.c_str(), valueName.data()))
		{
			for (size_t i = 0; i < Size; i++)
			{
				bool selected = size_t(value) == i;

				if (ImGui::Selectable(values[i].data(), selected))
				{
					value = T(i);
					modified = true;
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		return modified;
	}
}