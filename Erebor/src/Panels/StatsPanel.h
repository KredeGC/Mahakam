#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class StatsPanel
	{
	private:
		Timestep m_Frametime = 0.0f;
		bool m_Open = true;

	public:
		StatsPanel() = default;

		void OnUpdate(Timestep dt);

		void OnImGuiRender();
	};
}