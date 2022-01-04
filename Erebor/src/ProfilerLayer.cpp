#include "ProfilerLayer.h"

namespace Mahakam
{
	void ProfilerLayer::onImGuiRender()
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