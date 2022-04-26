#include "ebpch.h"
#include "StatsPanel.h"

namespace Mahakam::Editor
{
	void StatsPanel::OnUpdate(Timestep dt)
	{
		m_Frametime = dt;
	}

	void StatsPanel::OnImGuiRender()
	{
		if (m_Open)
		{
			auto& results = Renderer::GetPerformanceResults();

			ImGui::Begin("Performance Stats", &m_Open);
			ImGui::Text("Graphics unit: %s", GL::GetGraphicsVendor());
			ImGui::Text("Drawcalls: %d", results.drawCalls);
			ImGui::Text("Vertex count: %d", results.vertexCount);
			ImGui::Text("Tri count: %d", results.triCount);
			ImGui::Text("Frametime: %.3d fps (%.3f ms)", (int)(1.0f / m_Frametime), m_Frametime.GetMilliSeconds());
			ImGui::End();
		}
	}
}