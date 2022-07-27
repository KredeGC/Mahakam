#pragma once

#include <imgui.h>

#include <glm/glm.hpp>

namespace Mahakam::GUI
{
	constexpr float COLUMN_WIDTH = 100.0f;
	constexpr int MAX_STR_LEN = 256;

	bool DrawVec3Control(const char* label, glm::vec3& value, float resetValue = 0.0f, bool disabled = false);

	bool DrawColor3Edit(const char* label, glm::vec3& value, ImGuiColorEditFlags flags = 0);

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
}