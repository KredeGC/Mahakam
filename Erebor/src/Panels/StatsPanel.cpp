#include "ebpch.h"
#include "StatsPanel.h"

namespace Mahakam
{
	void StatsPanel::onUpdate(Timestep dt)
	{
		frametime = dt;
	}

	void StatsPanel::onImGuiRender()
	{
		if (open)
		{
			auto results = Renderer::getPerformanceResults();

			ImGui::Begin("Stats", &open);
			ImGui::Text("Graphics unit: %s", GL::getGraphicsVendor());
			ImGui::Text("Drawcalls: %d", results->drawCalls);
			ImGui::Text("Vertex count: %d", results->vertexCount);
			ImGui::Text("Tri count: %d", results->triCount);
			ImGui::Text("Frametime: %d fps (%.4g ms)", (int)(1.0f / frametime), frametime.getMilliSeconds());
			ImGui::End();
		}
	}
}