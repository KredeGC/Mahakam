#include "ebpch.h"
#include "ConsolePanel.h"

namespace Mahakam::Editor
{
	std::vector<ConsolePanel::ConsoleLine> ConsolePanel::m_Lines;

	void ConsolePanel::OnImGuiRender()
	{
		if (ImGui::Begin("Console", &m_Open))
		{
			for (auto& kv : m_Lines)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, kv.Color);
				ImGui::TextWrapped("%s", kv.Line.c_str());
				ImGui::PopStyleColor();
			}
		}

		ImGui::End();
	}

	void ConsolePanel::AddLog(const ImVec4& color, const std::string& msg)
	{
		m_Lines.push_back({ color, msg });
	}
}