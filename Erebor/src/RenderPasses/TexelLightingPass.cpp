#include "ebpch.h"
#include "TexelLightingPass.h"

#include "Mahakam/Core/Frustum.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Renderer.h"

namespace Mahakam
{
	void TexelLightingPass::Init(uint32_t width, uint32_t height)
	{
		brdfLut = LoadOrCreateLUTTexture("assets/textures/brdf.dat", "assets/shaders/internal/BRDF.yaml", TextureFormat::RG16F, 512, 512);
		falloffLut = LoadOrCreateLUTTexture("assets/textures/falloff.dat", "assets/shaders/internal/Falloff.yaml", TextureFormat::R16F, 16, 16);

		spotlightTexture = Texture2D::Create("assets/textures/internal/spotlight.png", { TextureFormat::SRGB_DXT1, TextureFilter::Bilinear, TextureWrapMode::ClampBorder, TextureWrapMode::ClampBorder });

		// Create HDR lighting framebuffer
		FrameBufferProps lightingProps;
		lightingProps.width = width;
		lightingProps.height = height;
		lightingProps.colorAttachments = { TextureFormat::RG11B10F };
		lightingProps.depthAttachment = { TextureFormat::Depth24 };

		hdrFrameBuffer = FrameBuffer::Create(lightingProps);

		// Create lighting shader
		deferredShader = Shader::Create("assets/shaders/external/DeferredTexel.yaml", { "DEBUG" });

		// Create shadow map
		FrameBufferProps shadowProps;
		shadowProps.width = shadowMapSize;
		shadowProps.height = shadowMapSize;
		shadowProps.depthAttachment = { TextureFormat::Depth24, TextureFilter::Point };
		shadowFramebuffer = FrameBuffer::Create(shadowProps);

		// Create default shadow shader
		shadowShader = Shader::Create("assets/shaders/internal/Shadow.yaml");

		shadowMatrixBuffer = UniformBuffer::Create(sizeof(glm::mat4));
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

		deferredShader->SetTexture("u_BRDFLUT", brdfLut);
		deferredShader->SetTexture("u_ShadowMap", shadowFramebuffer->GetDepthTexture());

		deferredShader->SetTexture("u_IrradianceMap", sceneData->environment.irradianceMap);
		deferredShader->SetTexture("u_SpecularMap", sceneData->environment.specularMap);
	}
}