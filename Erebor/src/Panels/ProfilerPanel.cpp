#include "ebpch.h"
#include "ProfilerPanel.h"

namespace Mahakam::Editor
{
	void ProfilerPanel::OnImGuiRender()
	{
		if (m_Open)
		{
			ImGui::Begin("Profiler", &m_Open);

			if (!m_Recording)
			{
				if (ImGui::Button("Record"))
				{
					m_Recording = true;
					uint64_t time = std::chrono::steady_clock::now().time_since_epoch().count();
					MH_PROFILE_BEGIN_SESSION("record", std::string("profiling/Record-") + std::to_string(time) + ".json");
				}
			}
			else
			{
				if (ImGui::Button("Stop"))
				{
					m_Recording = false;
					MH_PROFILE_END_SESSION();
				}
			}

			for (auto& result : Profiler::GetResults())
			{
				std::string viewName = "%.3fms  " + result.name;
				float milliseconds = result.elapsedTime.count() * 0.001f;

				ImGui::Text(viewName.c_str(), milliseconds);
			}

			ImGui::End();
		}
	}
}