#include "ebpch.h"
#include "ProfilerPanel.h"

namespace Mahakam
{
	void ProfilerPanel::OnImGuiRender()
	{
		if (open)
		{
			ImGui::Begin("Profiler", &open);

			if (!recording)
			{
				if (ImGui::Button("Record"))
				{
					recording = true;
					uint64_t time = std::chrono::steady_clock::now().time_since_epoch().count();
					MH_PROFILE_BEGIN_SESSION("record", std::string("Profiling-Record-") + std::to_string(time) + ".json");
				}
			}
			else
			{
				if (ImGui::Button("Stop"))
				{
					recording = false;
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

			Profiler::ClearResults();
		}
	}
}