#include "ebpch.h"
#include "BuildPanel.h"

namespace Mahakam::Editor
{
	void BuildPanel::OnImGuiRender()
	{
		if (ImGui::Begin("Build Runtime", &m_Open))
		{
			bool hasRun, hasStop, hasUpdate;
			
			if (Runtime::HasRuntime(hasRun, hasStop, hasUpdate))
			{
				ImGui::Text(u8"\ueed7" "  Runtime loaded correctly");

				ImGui::Text("%s  Run(Scene*)", hasRun ? u8"\ueed7" : u8"\ueedd");

				ImGui::Text("%s  Stop(Scene*)", hasStop ? u8"\ueed7" : u8"\ueedd");

				ImGui::Text("%s  Update(Scene*, Timestep)", hasUpdate ? u8"\ueed7" : u8"\ueedd");
			}
			else
			{
				ImGui::Text(u8"\ueedd" "  Runtime not loaded");
			}

			// TODO: Make button disabled in playmode
			if (ImGui::Button("Reload runtime"))
			{
				Runtime::ReloadRuntime();
			}
		}

		ImGui::End();
	}
}