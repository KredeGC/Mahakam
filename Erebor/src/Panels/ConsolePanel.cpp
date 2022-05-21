#include "ebpch.h"
#include "ConsolePanel.h"

namespace Mahakam::Editor
{
	std::vector<std::string> ConsolePanel::m_Lines;

	void ConsolePanel::OnImGuiRender()
	{
		if (ImGui::Begin("Console", &m_Open))
		{
			for (auto& line : m_Lines)
			{
				//ImGui::PushStyleColor(ImGuiCol_Text, { 1, 0, 0, 1 });
				ImGui::TextWrapped(line.c_str());
				//ImGui::PopStyleColor();
			}
		}

		ImGui::End();
	}

	void ConsolePanel::AddLog(const std::string& msg)
	{
		m_Lines.push_back(msg);
	}
}