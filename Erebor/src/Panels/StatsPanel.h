#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class StatsPanel
	{
	private:
		Timestep frametime = 0.0f;
		uint32_t* drawCalls;
		uint32_t* vertexCount;
		uint32_t* triCount;
		bool open = true;

	public:
		StatsPanel(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
			: drawCalls(drawCalls), vertexCount(vertexCount), triCount(triCount) {}

		void onUpdate(Timestep dt);

		void onImGuiRender();
	};
}