#pragma once

namespace Mahakam::Editor
{
	class ConsolePanel : public EditorWindow
	{
	private:
		bool m_Open = true;

		static std::vector<std::string> m_Lines;

	public:
		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;

		static void AddLog(const std::string& msg);
	};
}