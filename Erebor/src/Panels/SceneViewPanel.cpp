#include "ebpch.h"
#include "SceneViewPanel.h"

namespace Mahakam
{
	void SceneViewPanel::OnImGuiRender()
	{
		MH_PROFILE_FUNCTION();

		if (open)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Viewport", &open);
			focused = ImGui::IsWindowFocused();
			hovered = ImGui::IsWindowHovered();
			ImVec2 size = ImGui::GetContentRegionAvail();
			if (size.x != viewportSize.x || size.y != viewportSize.y)
			{
				viewportSize.x = size.x;
				viewportSize.y = size.y;

				activeScene->OnViewportResize((uint32_t)size.x, (uint32_t)size.y);
			}
			ImGui::Image((void*)(uintptr_t)viewportFramebuffer->GetColorBuffer(0)->GetRendererID(), size, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	bool SceneViewPanel::OnMouseScrolled(MouseScrolledEvent& event)
	{
		if (focused && hovered)
		{
			MH_CORE_TRACE("Scrolled!");
			return true;
		}

		return false;
	}

	void SceneViewPanel::SetContext(Ref<Scene> scene, Ref<FrameBuffer> framebuffer)
	{
		activeScene = scene;
		viewportFramebuffer = framebuffer;
	}
}