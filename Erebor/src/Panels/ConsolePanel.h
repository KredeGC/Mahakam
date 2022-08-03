#pragma once

#include <Mahakam.h>

#include <spdlog/common.h>

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
		bool m_ClearOnPlay = true;

		static inline bool s_ViewTrace = true;
		static inline bool s_ViewInfo = true;
		static inline bool s_ViewWarn = true;
		static inline bool s_ViewError = true;
		static inline bool s_ViewFatal = true;

		static std::vector<ConsoleLine> s_Lines;

	public:
		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;

		static void AddLog(spdlog::level::level_enum level, const std::string& msg);
	};
}