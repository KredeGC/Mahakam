#include "mhpch.h"
#include "TonemappingRenderPass.h"

#include "Mahakam/Renderer/GL.h"

namespace Mahakam
{
	TonemappingRenderPass::TonemappingRenderPass(uint32_t width, uint32_t height)
	{
		// Create viewport framebuffer
		FrameBufferProps viewportProps;
		viewportProps.width = width;
		viewportProps.height = height;
		viewportProps.colorAttachments = { TextureFormat::RGBA8 };

		viewportFramebuffer = FrameBuffer::Create(viewportProps);

		Ref<Shader> tonemappingShader = Shader::Create("assets/shaders/internal/Tonemapping.yaml");
		tonemappingMaterial = Material::Create(tonemappingShader);
	}

	TonemappingRenderPass::~TonemappingRenderPass()
	{
		viewportFramebuffer = nullptr;
		tonemappingMaterial = nullptr;
	}

	void TonemappingRenderPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		updateTextures = true;

		viewportFramebuffer->Resize(width, height);
	}

	bool TonemappingRenderPass::Render(Renderer::SceneData* sceneData, Ref<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (sceneData->wireframe)
		{
			src->Blit(viewportFramebuffer);
			return false;
		}

		if (updateTextures)
			tonemappingMaterial->SetTexture("u_Albedo", 0, src->GetColorTexture(0));

		src->Blit(viewportFramebuffer, false, true);

		viewportFramebuffer->Bind();
		GL::SetClearColor({ 1.0f, 0.06f, 0.94f, 1.0f });
		GL::Clear(true, false);

		// HDR Tonemapping
		tonemappingMaterial->BindShader("POSTPROCESSING");
		tonemappingMaterial->Bind();

		GL::EnableZTesting(false);
		GL::EnableZWriting(false);
		Renderer::DrawScreenQuad();
		GL::EnableZWriting(true);
		GL::EnableZTesting(true);

		viewportFramebuffer->Unbind();

		return true;
	}
}