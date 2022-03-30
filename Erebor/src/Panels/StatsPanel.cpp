#include "ebpch.h"
#include "StatsPanel.h"

namespace Mahakam
{
	void StatsPanel::OnUpdate(Timestep dt)
	{
		frametime = dt;
	}

	void StatsPanel::OnImGuiRender()
	{
		if (open)
		{
			auto& results = Renderer::GetPerformanceResults();

			ImGui::Begin("Performance Stats", &open);
			ImGui::Text("Graphics unit: %s", GL::GetGraphicsVendor());
			ImGui::Text("Drawcalls: %d", results.drawCalls);
			ImGui::Text("Vertex count: %d", results.vertexCount);
			ImGui::Text("Tri count: %d", results.triCount);
			ImGui::Text("Frametime: %.3d fps (%.3f ms)", (int)(1.0f / frametime), frametime.GetMilliSeconds());
			ImGui::End();
		}
	}
}