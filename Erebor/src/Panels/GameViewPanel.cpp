#include "ebpch.h"
#include "GameViewPanel.h"

namespace Mahakam
{
	void GameViewPanel::OnImGuiRender()
	{
		MH_PROFILE_FUNCTION();

		if (open)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Game View", &open);
			focused = ImGui::IsWindowFocused();
			hovered = ImGui::IsWindowHovered();
			ImVec2 size = ImGui::GetContentRegionAvail();
			if (size.x != viewportSize.x || size.y != viewportSize.y)
			{
				viewportSize.x = size.x;
				viewportSize.y = size.y;

				if (auto context = activeScene.lock())
					context->OnViewportResize((uint32_t)size.x, (uint32_t)size.y);
			}
			if (viewportTexture)
				ImGui::Image((void*)(uintptr_t)viewportTexture->GetRendererID(), size, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	bool GameViewPanel::OnMouseScrolled(MouseScrolledEvent& event)
	{
		if (focused && hovered)
		{
			MH_CORE_TRACE("Scrolled!");
			return true;
		}

		return false;
	}

	void GameViewPanel::SetScene(WeakRef<Scene> scene)
	{
		activeScene = scene;
	}

	void GameViewPanel::SetFrameBuffer(Ref<Texture> tex)
	{
		viewportTexture = tex;
	}
}