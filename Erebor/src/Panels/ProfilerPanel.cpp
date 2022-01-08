#include "ebpch.h"
#include "ProfilerPanel.h"

namespace Mahakam
{
	void ProfilerPanel::onImGuiRender()
	{
		if (open)
		{
			ImGui::Begin("Profiler", &open);

			for (auto& result : Profiler::getResults())
			{
				char label[256];
				strcpy(label, "%.3fms  ");
				strcat(label, result.name);
				ImGui::Text(label, result.duration);
			}

			ImGui::End();

			Profiler::clearResults();
		}
	}
}