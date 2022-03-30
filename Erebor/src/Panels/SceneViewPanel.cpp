#include "ebpch.h"
#include "SceneViewPanel.h"

namespace Mahakam
{
	void SceneViewPanel::OnUpdate(Timestep dt)
	{
		if (open)
			editorCamera.OnUpdate(dt, focused, hovered);
	}

	void SceneViewPanel::OnImGuiRender()
	{
		MH_PROFILE_FUNCTION();

		if (open)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Scene View", &open);
			focused = ImGui::IsWindowFocused();
			hovered = ImGui::IsWindowHovered();
			Application::GetInstance().GetImGuiLayer()->BlockEvents(!focused && !hovered);
			ImVec2 size = ImGui::GetContentRegionAvail();
			if (size.x != viewportSize.x || size.y != viewportSize.y)
			{
				viewportSize.x = size.x;
				viewportSize.y = size.y;

				editorCamera.GetCamera().SetRatio(viewportSize.x / viewportSize.y);
			}
			if (viewportTexture)
				ImGui::Image((void*)(uintptr_t)viewportTexture->GetRendererID(), size, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	void SceneViewPanel::OnEvent(Event& event)
	{
		if (hovered)
		{
			EventDispatcher dispatcher(event);
			dispatcher.DispatchEvent<MouseScrolledEvent>(MH_BIND_EVENT(editorCamera.OnMouseScroll));
		}
	}
}