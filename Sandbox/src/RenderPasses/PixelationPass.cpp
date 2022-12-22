#include "sbpch.h"
#include "PixelationPass.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Renderer.h"

namespace Mahakam
{
	bool PixelationPass::Init(uint32_t width, uint32_t height)
	{
		if (RenderPass::Init(width, height))
			return true;

		// Create viewport framebuffer
		FrameBufferProps viewportProps;
		viewportProps.Width = width;
		viewportProps.Height = height;
		viewportProps.ColorAttachments = TrivialVector<FrameBufferAttachmentProps>{ TextureFormat::RGBA8 };

		viewportFramebuffer = FrameBuffer::Create(viewportProps);

		pixelationShader = Shader::Create("assets/shaders/external/Pixelation.shader");

		return true;
	}

	PixelationPass::~PixelationPass()
	{
		viewportFramebuffer = nullptr;
		pixelationShader = nullptr;
	}

	void PixelationPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		viewportFramebuffer->Resize(width, height);
	}

	bool PixelationPass::Render(SceneData* sceneData, const Asset<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (sceneData->wireframe)
		{
			src->Blit(viewportFramebuffer);
			return false;
		}

		src->Blit(viewportFramebuffer, false, true);

		viewportFramebuffer->Bind();
		GL::SetClearColor({ 1.0f, 0.06f, 0.94f, 1.0f });
		GL::Clear(true, false);

		// HDR Tonemapping
		pixelationShader->Bind("POSTPROCESSING");
		pixelationShader->SetTexture("u_Albedo", src->GetColorTexture(0));
		pixelationShader->SetTexture("u_Depth", src->GetDepthTexture());

		GL::EnableZTesting(false);
		GL::EnableZWriting(false);
		Renderer::DrawScreenQuad();
		GL::EnableZWriting(true);
		GL::EnableZTesting(true);

		viewportFramebuffer->Unbind();

		return true;
	}
}