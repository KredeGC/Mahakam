#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>

namespace Mahakam::GUI
{
	constexpr float COLUMN_WIDTH = 100.0f;

	bool DrawVec3Control(const char* label, glm::vec3& value, float resetValue = 0.0f);

	bool DrawColor3Edit(const char* label, glm::vec3& value, ImGuiColorEditFlags flags = 0);
}