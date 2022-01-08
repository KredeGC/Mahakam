#include "ebpch.h"
#include "SceneViewPanel.h"

namespace Mahakam
{
	void SceneViewPanel::onImGuiRender()
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

				// TODO: Find a better place for this
				//camera.setRatio(size.x / size.y);

				activeScene->onViewportResize((uint32_t)size.x, (uint32_t)size.y);
			}
			ImGui::Image((void*)viewportFramebuffer->getColorAttachments()[0]->getRendererID(), size, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	bool SceneViewPanel::onMouseScrolled(MouseScrolledEvent& event)
	{
		if (focused && hovered)
		{
			MH_CORE_TRACE("Scrolled!");
			return true;
		}

		return false;
	}

	void SceneViewPanel::setContext(const Ref<Scene>& scene, const Ref<FrameBuffer>& framebuffer)
	{
		activeScene = scene;
		viewportFramebuffer = framebuffer;
	}
}