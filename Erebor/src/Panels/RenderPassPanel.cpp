#include "ebpch.h"
#include "RenderPassPanel.h"

namespace Mahakam::Editor
{
	static void DrawComboBox(const char* label, const std::vector<std::string>& names, int& index)
	{
		// Align everything with columns
		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100.0f);

		ImGui::Text("%s", label);
		ImGui::NextColumn();

		// Show combo box
		if (ImGui::BeginCombo("", names[index].c_str()))
		{
			for (int n = 0; n < names.size(); n++)
			{
				const bool is_selected = (index == n);
				if (ImGui::Selectable(names[n].c_str(), is_selected))
					index = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		// Remember to pop
		ImGui::Columns(1);

		ImGui::PopID();
	}

	RenderPassPanel::RenderPassPanel()
	{
		FrameBufferProps viewportProps;
		viewportProps.Width = 1024;
		viewportProps.Height = 1024;
		viewportProps.ColorAttachments = { TextureFormat::RGB8 };
		viewportProps.DepthAttachment = TextureFormat::Depth24;

		m_ViewportFramebuffer = FrameBuffer::Create(viewportProps);

		m_BlitShader = Shader::Create("internal/shaders/builtin/Blit.shader");
	}

	void RenderPassPanel::OnImGuiRender()
	{
		if (m_Open)
		{
			if (ImGui::Begin("Renderpasses", &m_Open))
				RenderPanel();

			ImGui::End();
		}
	}

	void RenderPassPanel::RenderPanel()
	{
		// Get a list of active framebuffers
		auto& frameBuffers = Renderer::GetFrameBuffers();

		if (frameBuffers.size() == 0) return;

		std::vector<std::string> frameBufferNames;
		for (auto& kv : frameBuffers)
			frameBufferNames.push_back(kv.first);

		if (m_FrameBufferIndex > frameBuffers.size())
			m_FrameBufferIndex = 0;

		// Show framebuffer combo box
		DrawComboBox("Framebuffer", frameBufferNames, m_FrameBufferIndex);

		if (auto frameBuffer = frameBuffers.at(frameBufferNames[m_FrameBufferIndex]))
		{
			auto& spec = frameBuffer->GetSpecification();

			// Get a list of mutable rendertextures
			std::vector<std::string> textureNames;
			std::vector<int> textures;
			int colorCount = (int)spec.ColorAttachments.size();
			for (int i = 0; i < colorCount; i++)
			{
				if (!spec.ColorAttachments[i].Immutable)
				{
					textures.push_back(i);
					textureNames.push_back("Color-" + std::to_string(i));
				}
			}

			if (!spec.DontUseDepth && !spec.DepthAttachment.Immutable)
			{
				textures.push_back(-1);
				textureNames.push_back("Depth");
			}

			if (m_TextureIndex > textures.size())
				m_TextureIndex = 0;

			if (textures.size() == 0)
			{
				ImGui::Text("This framebuffer has no viewable textures");
			}
			else
			{
				DrawComboBox("Texture", textureNames, m_TextureIndex);

				RenderImage(frameBuffer, textures[m_TextureIndex], textures[m_TextureIndex] == -1);
			}
		}
	}

	void RenderPassPanel::RenderImage(Asset<FrameBuffer> frameBuffer, int index, bool depth)
	{
		m_ViewportFramebuffer->Bind();

		m_BlitShader->Bind("POSTPROCESSING");
		if (depth)
			m_BlitShader->SetTexture("u_Albedo", frameBuffer->GetDepthTexture());
		else
			m_BlitShader->SetTexture("u_Albedo", frameBuffer->GetColorTexture(index));

		m_BlitShader->SetUniformInt("u_Depth", depth);

		GL::EnableZTesting(false);
		GL::EnableZWriting(false);
		Renderer::DrawScreenQuad();
		GL::EnableZWriting(true);
		GL::EnableZTesting(true);

		m_ViewportFramebuffer->Unbind();

		ImVec2 contentSize = ImGui::GetContentRegionAvail();
		float maxSize = std::min(contentSize.x, contentSize.y);

		ImGui::Image((void*)(uintptr_t)m_ViewportFramebuffer->GetColorTexture(0)->GetRendererID(), ImVec2(maxSize, maxSize), ImVec2(0, 1), ImVec2(1, 0));
	}
}