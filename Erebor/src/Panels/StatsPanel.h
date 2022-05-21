#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class StatsPanel : EditorWindow
	{
	private:
		Timestep m_Frametime = 0.0f;
		bool m_Open = true;

	public:
		StatsPanel() = default;

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnUpdate(Timestep dt) override;

		virtual void OnImGuiRender() override;
	};
}