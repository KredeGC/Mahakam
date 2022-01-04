#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class StatsLayer : public Layer
	{
	private:
		Timestep frametime = 0.0f;
		uint32_t* drawCalls;
		uint32_t* vertexCount;
		uint32_t* triCount;
		bool open = true;

	public:
		StatsLayer(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
			: drawCalls(drawCalls), vertexCount(vertexCount), triCount(triCount), Layer("Stats") {}

		virtual void onUpdate(Timestep dt) override;

		virtual void onImGuiRender() override;
	};
}