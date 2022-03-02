#include "mhpch.h"
#include "TonemappingRenderPass.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Renderer.h"

namespace Mahakam
{
	void TonemappingRenderPass::Init(uint32_t width, uint32_t height)
	{
		// Create viewport framebuffer
		FrameBufferProps viewportProps;
		viewportProps.width = width;
		viewportProps.height = height;
		viewportProps.colorAttachments = { TextureFormat::RGBA8 };

		viewportFramebuffer = FrameBuffer::Create(viewportProps);

		tonemappingShader = Shader::Create("assets/shaders/internal/Tonemapping.yaml");
	}

	TonemappingRenderPass::~TonemappingRenderPass()
	{
		viewportFramebuffer = nullptr;
		tonemappingShader = nullptr;
	}

	void TonemappingRenderPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		viewportFramebuffer->Resize(width, height);
	}

	bool TonemappingRenderPass::Render(SceneData* sceneData, Ref<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (sceneData->wireframe)
			return false;

		src->Blit(viewportFramebuffer, false, true);

		viewportFramebuffer->Bind();
		GL::SetClearColor({ 1.0f, 0.06f, 0.94f, 1.0f });
		GL::Clear(true, false);

		// HDR Tonemapping
		tonemappingShader->Bind("POSTPROCESSING");
		tonemappingShader->SetTexture("u_Albedo", src->GetColorTexture(0));

		GL::EnableZTesting(false);
		GL::EnableZWriting(false);
		Renderer::DrawScreenQuad();
		GL::EnableZWriting(true);
		GL::EnableZTesting(true);

		viewportFramebuffer->Unbind();

		return true;
	}
}