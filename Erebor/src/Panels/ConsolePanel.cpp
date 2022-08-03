#include "ebpch.h"
#include "ConsolePanel.h"

namespace Mahakam::Editor
{
	std::vector<ConsolePanel::ConsoleLine> ConsolePanel::s_Lines;

	void ConsolePanel::OnImGuiRender()
	{
		if (ImGui::Begin("Console", &m_Open, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::MenuItem("Clear"))
					s_Lines.clear();
				
				if (ImGui::BeginMenu("View"))
				{
					if (ImGui::MenuItem("Trace", nullptr, &s_ViewTrace)) { }

					if (ImGui::MenuItem("Info", nullptr, &s_ViewInfo)) { }

					if (ImGui::MenuItem("Warn", nullptr, &s_ViewWarn)) { }

					if (ImGui::MenuItem("Error", nullptr, &s_ViewError)) { }

					if (ImGui::MenuItem("Fatal", nullptr, &s_ViewFatal)) { }
					
					ImGui::EndMenu();
				}
				
				if (ImGui::MenuItem("Clear on play", nullptr, &m_ClearOnPlay)) { }
				
				ImGui::EndMenuBar();
			}
			
			for (auto& kv : s_Lines)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, kv.Color);
				ImGui::TextWrapped("%s", kv.Line.c_str());
				ImGui::PopStyleColor();
			}
		}

		ImGui::End();
	}

	void ConsolePanel::AddLog(spdlog::level::level_enum level, const std::string& msg)
	{
		switch (level)
		{
		case spdlog::level::level_enum::trace:
			if (s_ViewTrace)
				s_Lines.push_back({ { 1.0f, 1.0f, 1.0f, 1.0f }, msg });
			break;
		case spdlog::level::level_enum::info:
			if (s_ViewInfo)
				s_Lines.push_back({ { 0.0f, 0.8f, 0.0f, 1.0f }, msg });
			break;
		case spdlog::level::level_enum::warn:
			if (s_ViewWarn)
				s_Lines.push_back({ { 1.0f, 1.0f, 0.0f, 1.0f }, msg });
			break;
		case spdlog::level::level_enum::err:
			if (s_ViewError)
				s_Lines.push_back({ { 1.0f, 0.0f, 0.0f, 1.0f }, msg });
			break;
		case spdlog::level::level_enum::critical:
			if (s_ViewFatal)
				s_Lines.push_back({ { 1.0f, 0.0f, 0.0f, 1.0f }, msg });
			break;
		}
	}
}