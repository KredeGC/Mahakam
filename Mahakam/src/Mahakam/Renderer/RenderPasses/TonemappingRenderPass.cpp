#include "Mahakam/mhpch.h"
#include "TonemappingRenderPass.h"

#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/RenderData.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/Shader.h"

namespace Mahakam
{
	bool TonemappingRenderPass::Init(uint32_t width, uint32_t height)
	{
		if (RenderPass::Init(width, height))
			return true;

		MH_PROFILE_RENDERING_FUNCTION();

		// Create viewport framebuffer
		FrameBufferProps viewportProps;
		viewportProps.Width = width;
		viewportProps.Height = height;
		viewportProps.ColorAttachments = TrivialVector<FrameBufferAttachmentProps>{ TextureFormat::RGBA8 };

		m_ViewportFramebuffer = FrameBuffer::Create(viewportProps);

		Renderer::AddFrameBuffer("Tonemapping", m_ViewportFramebuffer);

		m_TonemappingShader = Shader::Create("internal/shaders/builtin/Tonemapping.shader");

		return true;
	}

	TonemappingRenderPass::~TonemappingRenderPass()
	{
		MH_PROFILE_FUNCTION();

		m_ViewportFramebuffer = nullptr;
		m_TonemappingShader = nullptr;
	}

	void TonemappingRenderPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		m_ViewportFramebuffer->Resize(width, height);
	}

	bool TonemappingRenderPass::Render(SceneData* sceneData, const Asset<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (sceneData->wireframe)
			return false;

		src->Blit(m_ViewportFramebuffer, false, true);

		m_ViewportFramebuffer->Bind();
		GL::SetClearColor({ 1.0f, 0.06f, 0.94f, 1.0f });
		GL::Clear(true, false);

		// HDR Tonemapping
		m_TonemappingShader->Bind("POSTPROCESSING");
		m_TonemappingShader->SetTexture("u_Albedo", src->GetColorTexture(0));

		GL::EnableZTesting(false);
		GL::EnableZWriting(false);
		Renderer::DrawScreenQuad();
		GL::EnableZWriting(true);
		GL::EnableZTesting(true);

		m_ViewportFramebuffer->Unbind();

		return true;
	}
}