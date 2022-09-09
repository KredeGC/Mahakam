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
		bool m_UpdateHigh = false;

		float m_Time = 1.0f;
		float m_SmoothDelta = 0.0f;

		TrivialVector<Profiler::ProfileResult> m_AverageResults;

	public:
		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnUpdate(Timestep dt) override;
		virtual void OnImGuiRender() override;

	private:
		void UpdateProfilerResults();
	};
}