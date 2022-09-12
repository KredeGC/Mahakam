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
		deferredShader = Shader::Create("assets/shaders/external/DeferredTexel.shader", { "DEBUG" });

		// Create default shadow shader
		shadowShader = Shader::Create("assets/shaders/internal/Shadow.shader");
	}

	void TexelLightingPass::SetupTextures(SceneData* sceneData, Ref<FrameBuffer> src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (sceneData->gBuffer)
			deferredShader->Bind("DIRECTIONAL", "DEBUG");
		else
			deferredShader->Bind("DIRECTIONAL");
		deferredShader->SetTexture("u_GBuffer0", src->GetColorTexture(0));
		deferredShader->SetTexture("u_GBuffer1", src->GetColorTexture(1));
		deferredShader->SetTexture("u_GBuffer3", src->GetColorTexture(3));
		deferredShader->SetTexture("u_GBuffer4", src->GetColorTexture(4));
		deferredShader->SetTexture("u_Depth", src->GetDepthTexture());

		deferredShader->SetTexture("u_BRDFLUT", Asset<Texture>(brdfLut));
		deferredShader->SetTexture("u_ShadowMap", shadowFramebuffer->GetDepthTexture());

		deferredShader->SetTexture("u_IrradianceMap", sceneData->environment.IrradianceMap);
		deferredShader->SetTexture("u_SpecularMap", sceneData->environment.SpecularMap);
	}
}