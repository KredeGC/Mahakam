#include "ebpch.h"
#include "StatsPanel.h"

namespace Mahakam::Editor
{
	void StatsPanel::OnUpdate(Timestep dt)
	{
		m_Frametime = dt;
		m_LastSample += dt;

		m_AvgFrametime = m_AvgFrametime + dt;
		m_NumFrames++;

		if (m_LastSample > 1.0f)
		{
			m_ShownFrametime = m_AvgFrametime / m_NumFrames;

			m_AvgFrametime = 0.0f;
			m_LastSample -= 1.0f;
			m_NumFrames = 0;
		}
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
				ImGui::TextWrapped("Average Frametime: %.3d fps (%.3f ms)", (int)(1.0f / m_ShownFrametime), m_ShownFrametime.GetMilliSeconds());
			}
			ImGui::End();
		}
	}
}