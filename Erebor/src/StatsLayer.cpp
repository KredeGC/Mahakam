#include "StatsLayer.h"

namespace Mahakam
{
	void StatsLayer::onUpdate(Timestep dt)
	{
		frametime = dt;
	}

	void StatsLayer::onImGuiRender()
	{
		if (open)
		{
			ImGui::Begin("Stats", &open);
			ImGui::Text("Graphics unit: %s", GL::getGraphicsVendor());
			ImGui::Text("Drawcalls: %d", *drawCalls);
			ImGui::Text("Vertex count: %d", *vertexCount);
			ImGui::Text("Tri count: %d", *triCount);
			ImGui::Text("Frametime: %d fps (%.4g ms)", (int)(1.0f / frametime), frametime.getMilliSeconds());
			ImGui::End();
		}
	}
}