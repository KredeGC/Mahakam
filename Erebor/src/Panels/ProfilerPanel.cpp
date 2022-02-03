#include "ebpch.h"
#include "ProfilerPanel.h"

namespace Mahakam
{
	void ProfilerPanel::OnImGuiRender()
	{
		if (open)
		{
			ImGui::Begin("Profiler", &open);

			for (auto& result : Profiler::GetResults())
			{
				char label[256];
				strcpy(label, "%.3fms  ");
				strcat(label, result.name);
				ImGui::Text(label, result.duration);
			}

			ImGui::End();

			Profiler::ClearResults();
		}
	}
}