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
		lightingProps.ColorAttachments = { TextureFormat::RG11B10F };
		lightingProps.DepthAttachment = { TextureFormat::Depth24 };

		hdrFrameBuffer = FrameBuffer::Create(lightingProps);
	}

	void TexelLightingPass::SetupShaders()
	{
		// Create lighting shader
		deferredShader = Shader::Create("assets/shaders/external/DeferredTexel.shader");

		// Create default shadow shader
		shadowShader = Shader::Create("assets/shaders/internal/Shadow.shader");
	}

	void TexelLightingPass::SetupTextures(SceneData* sceneData, Ref<FrameBuffer> src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		deferredShader->Bind("DIRECTIONAL");
		deferredShader->SetTexture("u_GBuffer0", src->GetColorTexture(0).RefPtr());
		deferredShader->SetTexture("u_GBuffer1", src->GetColorTexture(1).RefPtr());
		deferredShader->SetTexture("u_GBuffer3", src->GetColorTexture(3).RefPtr());
		deferredShader->SetTexture("u_GBuffer4", src->GetColorTexture(4).RefPtr());
		deferredShader->SetTexture("u_Depth", src->GetDepthTexture().RefPtr());

		deferredShader->SetTexture("u_BRDFLUT", brdfLut);
		deferredShader->SetTexture("u_ShadowMap", shadowFramebuffer->GetDepthTexture().RefPtr());

		deferredShader->SetTexture("u_IrradianceMap", sceneData->environment.IrradianceMap.RefPtr());
		deferredShader->SetTexture("u_SpecularMap", sceneData->environment.SpecularMap.RefPtr());
	}
}