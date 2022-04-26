#include "ebpch.h"
#include "GameViewPanel.h"

#include "EditorLayer.h"

namespace Mahakam::Editor
{
	void GameViewPanel::OnImGuiRender()
	{
		MH_PROFILE_FUNCTION();

		if (m_Open)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Game View", &m_Open);
			m_Focused = ImGui::IsWindowFocused();
			m_Hovered = ImGui::IsWindowHovered();
			ImVec2 size = ImGui::GetContentRegionAvail();
			if (size.x != m_ViewportSize.x || size.y != m_ViewportSize.y)
			{
				m_ViewportSize.x = size.x;
				m_ViewportSize.y = size.y;

				EditorLayer::GetActiveScene()->OnViewportResize((uint32_t)size.x, (uint32_t)size.y);
			}

			Ref<FrameBuffer> framebuffer = Renderer::GetFrameBuffer();
			if (framebuffer)
			{
				Ref<Texture> viewportTexture = framebuffer->GetColorTexture(0);
				if (viewportTexture)
					ImGui::Image((void*)(uintptr_t)viewportTexture->GetRendererID(), size, ImVec2(0, 1), ImVec2(1, 0));
			}

			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	bool GameViewPanel::OnMouseScrolled(MouseScrolledEvent& event)
	{
		if (m_Focused && m_Hovered)
		{
			MH_CORE_TRACE("Scrolled!");
			return true;
		}

		return false;
	}
}