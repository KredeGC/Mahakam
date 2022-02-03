#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class StatsPanel
	{
	private:
		Timestep frametime = 0.0f;
		bool open = true;

	public:
		StatsPanel() = default;

		void OnUpdate(Timestep dt);

		void OnImGuiRender();
	};
}