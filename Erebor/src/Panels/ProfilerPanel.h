#pragma once

#include <Mahakam/Mahakam.h>

#include <imgui/imgui.h>

namespace Mahakam::Editor
{
	class ProfilerPanel : EditorWindow
	{
	private:
		bool m_Open = true;
		bool m_Recording = false;

	public:
		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;
	};
}