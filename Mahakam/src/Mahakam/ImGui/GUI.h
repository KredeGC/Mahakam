#pragma once

#include "Mahakam/Asset/Asset.h"

#ifndef MH_STANDALONE
#include "Mahakam/Editor/Resource/ResourceRegistry.h"
#endif

#include <imgui/imgui.h>

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

#include <array>
#include <charconv>
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

	bool DrawDragDropAssetField(const std::string& label, const std::vector<std::string>& extensions, std::filesystem::path& importPath);
	bool AcceptPayloadTarget(std::filesystem::path& importPath, std::string_view target);
	bool AcceptPayloadTarget(AssetID& id, std::string_view target);

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

	template<typename T, typename... Ts>
	bool DrawDragDropTarget(T& id, Ts&&... extensions)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if ((AcceptPayloadTarget(id, std::forward<Ts>(extensions)) || ...))
				return true;

			ImGui::EndDragDropTarget();
		}

		return false;
	}

	template<typename... Ts>
	bool DrawDragDropField(const std::string& label, std::filesystem::path& importPath, Ts&&... extensions)
	{
		std::string importString = FileUtility::Relative(importPath).generic_string();
		char filepathBuffer[MAX_STR_LEN]{ 0 };
		strncpy(filepathBuffer, importString.c_str(), importString.size());
		if (ImGui::InputText(label.c_str(), filepathBuffer, MAX_STR_LEN))
		{
			importPath = FileUtility::PROJECT_PATH / std::string(filepathBuffer);

			return true;
		}

		if (DrawDragDropTarget(importPath, std::forward<Ts>(extensions) ...))
		{
			return true;
		}

		return false;
	}

	template<typename T, typename... Ts>
	bool DrawDragDropAsset(const std::string& label, Asset<T>& value, Ts&&... extensions)
	{
#ifndef MH_STANDALONE
		ResourceRegistry::ImportInfo info = ResourceRegistry::GetImportInfo(value.GetID());

		std::string importString = FileUtility::Relative(info.Filepath).generic_string();
#else // MH_STANDALONE
		std::string importString = std::to_string(value.GetID());
#endif // MH_STANDALONE

		char filepathBuffer[MAX_STR_LEN]{ 0 };
		strncpy(filepathBuffer, importString.c_str(), importString.size());
		if (ImGui::InputText(label.c_str(), filepathBuffer, MAX_STR_LEN))
		{
			std::string pathString = filepathBuffer;
			AssetID id;
			if (std::from_chars(pathString.data(), pathString.data() + pathString.size(), id).ec == std::errc{})
			{
				value = Asset<T>(id);
			}
#ifndef MH_STANDALONE
			else
			{
				info = ResourceRegistry::GetImportInfo(FileUtility::PROJECT_PATH / pathString);
				value = Asset<T>(info.ID);
			}
#endif // MH_STANDALONE

			return true;
		}

		AssetID id;
		if (DrawDragDropTarget(id, std::forward<Ts>(extensions) ...))
		{
			value = Asset<T>(id);
			return true;
		}

		return false;
	}

	template<size_t Size>
	bool DrawDragDropField(const std::string& label, const std::array<std::string_view, Size>& extensions, std::filesystem::path& importPath)
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

		bool open;
		if constexpr (std::is_enum_v<T>)
			open = ImGui::BeginCombo(label.c_str(), values[std::underlying_type_t<T>(value)].data());
		else
			open = ImGui::BeginCombo(label.c_str(), values[value].data());

		if (open)
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