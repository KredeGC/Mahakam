#include "ebpch.h"
#include "StatsPanel.h"

namespace Mahakam::Editor
{
	void StatsPanel::OnUpdate(Timestep dt)
	{
		m_Frametime = dt;

		m_SmoothDelta = 0.99f * m_SmoothDelta + 0.01f * dt;
	}

	void StatsPanel::OnImGuiRender()
	{
		if (m_Open)
		{
			auto& results = Renderer::GetPerformanceResults();

			if (ImGui::Begin("Performance Stats", &m_Open))
			{
				ImGui::TextWrapped("Graphics unit: %s", GL::GetGraphicsVendor());
				ImGui::TextWrapped("Drawcalls: %d", results.DrawCalls);
				ImGui::TextWrapped("Vertex count: %d", results.VertexCount);
				ImGui::TextWrapped("Tri count: %d", results.TriCount);
				ImGui::TextWrapped("Frametime: %.3d fps (%.3f ms)", (int)(1.0f / m_Frametime), m_Frametime.GetMilliSeconds());
				ImGui::TextWrapped("Smooth Frametime: %.3d fps (%.3f ms)", (int)(1.0f / m_SmoothDelta), m_SmoothDelta.GetMilliSeconds());
			}
			ImGui::End();
		}
	}
}