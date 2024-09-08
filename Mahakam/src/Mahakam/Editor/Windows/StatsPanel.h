#pragma once

#include <Mahakam/Mahakam.h>

#include <imgui/imgui.h>

namespace Mahakam::Editor
{
	class StatsPanel : EditorWindow
	{
	private:
		Timestep m_Frametime = 0.0f;

		float m_LastSample = 0.0f;
		float m_AvgFrametime = 0.0f;
		size_t m_NumFrames = 0;
		Timestep m_ShownFrametime = 0.0f;

		bool m_Open = true;

	public:
		StatsPanel() = default;

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnUpdate(Timestep dt) override;

		virtual void OnImGuiRender() override;
	};
}