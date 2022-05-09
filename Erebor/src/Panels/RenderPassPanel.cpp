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

		ImGui::Text(label);
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
		viewportProps.width = 1024;
		viewportProps.height = 1024;
		viewportProps.colorAttachments = { TextureFormat::RGB8 };
		viewportProps.depthAttachment = TextureFormat::Depth24;

		viewportFramebuffer = FrameBuffer::Create(viewportProps);

		blitShader = Shader::Create("assets/shaders/internal/Blit.shader");
	}

	void RenderPassPanel::OnImGuiRender()
	{
		if (open)
		{
			ImGui::Begin("Renderpasses", &open);

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

		if (frameBufferIndex > frameBuffers.size())
			frameBufferIndex = 0;

		// Show framebuffer combo box
		DrawComboBox("Framebuffer", frameBufferNames, frameBufferIndex);

		if (auto frameBuffer = frameBuffers.at(frameBufferNames[frameBufferIndex]).lock())
		{
			auto& spec = frameBuffer->GetSpecification();

			// Get a list of mutable rendertextures
			std::vector<std::string> textureNames;
			std::vector<int> textures;
			int colorCount = (int)spec.colorAttachments.size();
			for (int i = 0; i < colorCount; i++)
			{
				if (!spec.colorAttachments[i].immutable)
				{
					textures.push_back(i);
					textureNames.push_back("Color-" + std::to_string(i));
				}
			}

			if (!spec.dontUseDepth && !spec.depthAttachment.immutable)
			{
				textures.push_back(-1);
				textureNames.push_back("Depth");
			}

			if (textureIndex > textures.size())
				textureIndex = 0;

			if (textures.size() == 0)
			{
				ImGui::Text("This framebuffer has no viewable textures");
			}
			else
			{
				DrawComboBox("Texture", textureNames, textureIndex);

				RenderImage(frameBuffer, textures[textureIndex], textures[textureIndex] == -1);
			}
		}
	}

	void RenderPassPanel::RenderImage(Asset<FrameBuffer> frameBuffer, int index, bool depth)
	{
		viewportFramebuffer->Bind();

		blitShader->Bind("POSTPROCESSING");
		if (depth)
			blitShader->SetTexture("u_Albedo", frameBuffer->GetDepthTexture());
		else
			blitShader->SetTexture("u_Albedo", frameBuffer->GetColorTexture(index));

		blitShader->SetUniformInt("u_Depth", depth);

		GL::EnableZTesting(false);
		GL::EnableZWriting(false);
		Renderer::DrawScreenQuad();
		GL::EnableZWriting(true);
		GL::EnableZTesting(true);

		viewportFramebuffer->Unbind();

		ImVec2 contentSize = ImGui::GetContentRegionAvail();
		float maxSize = std::min(contentSize.x, contentSize.y);

		ImGui::Image((void*)(uintptr_t)viewportFramebuffer->GetColorTexture(0)->GetRendererID(), ImVec2(maxSize, maxSize), ImVec2(0, 1), ImVec2(1, 0));
	}
}