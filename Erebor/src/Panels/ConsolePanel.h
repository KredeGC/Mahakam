#pragma once

#include <imgui.h>

namespace Mahakam::Editor
{
	class ConsolePanel : public EditorWindow
	{
	public:
		struct ConsoleLine
		{
			ImVec4 Color;
			std::string Line;
		};

	private:
		bool m_Open = true;

		static std::vector<ConsoleLine> m_Lines;

	public:
		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;

		static void AddLog(const ImVec4& color, const std::string& msg);
	};
}