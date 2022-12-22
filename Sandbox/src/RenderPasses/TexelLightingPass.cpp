#include "sbpch.h"
#include "TexelLightingPass.h"

#include "Mahakam/Core/Frustum.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Renderer.h"

namespace Mahakam
{
	void TexelLightingPass::SetupFrameBuffer(uint32_t width, uint32_t height)
	{
		// Create HDR lighting framebuffer
		FrameBufferProps lightingProps;
		lightingProps.Width = width;
		lightingProps.Height = height;
		lightingProps.ColorAttachments = TrivialVector<FrameBufferAttachmentProps>{ TextureFormat::RG11B10F };
		lightingProps.DepthAttachment = { TextureFormat::Depth24 };

		m_HDRFrameBuffer = FrameBuffer::Create(lightingProps);
	}

	void TexelLightingPass::SetupShaders()
	{
		// Create lighting shader
		m_DeferredShader = Shader::Create("assets/shaders/external/DeferredTexel.shader");

		// Create default shadow shader
		m_ShadowShader = Shader::Create("assets/shaders/internal/Shadow.shader");
	}

	void TexelLightingPass::SetupTextures(SceneData* sceneData, const Asset<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		m_DeferredShader->Bind("DIRECTIONAL");
		m_DeferredShader->SetTexture("u_GBuffer0", src->GetColorTexture(0));
		m_DeferredShader->SetTexture("u_GBuffer1", src->GetColorTexture(1));
		m_DeferredShader->SetTexture("u_GBuffer3", src->GetColorTexture(3));
		m_DeferredShader->SetTexture("u_GBuffer4", src->GetColorTexture(4));
		m_DeferredShader->SetTexture("u_Depth", src->GetDepthTexture());
		
		m_DeferredShader->SetTexture("u_BRDFLUT", m_BRDFLut);
		m_DeferredShader->SetTexture("u_ShadowMap", m_ShadowFramebuffer->GetDepthTexture());
		
		m_DeferredShader->SetTexture("u_IrradianceMap", sceneData->environment.IrradianceMap);
		m_DeferredShader->SetTexture("u_SpecularMap", sceneData->environment.SpecularMap);
	}
}