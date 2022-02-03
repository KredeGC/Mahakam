#include "mhpch.h"
#include "LightingRenderPass.h"

#include "Mahakam/Renderer/GL.h"

#include "Mahakam/Renderer/Texture.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Mahakam
{
	LightingRenderPass::LightingRenderPass(uint32_t width, uint32_t height)
	{
		brdfLut = LoadOrCreateLUTTexture("assets/textures/brdf.dat", "assets/shaders/internal/BRDF.yaml", TextureFormat::RG16F, 512, 512);
		falloffLut = LoadOrCreateLUTTexture("assets/textures/falloff.dat", "assets/shaders/internal/Falloff.yaml", TextureFormat::R16F, 16, 16);

		spotlightTexture = Texture2D::Create("assets/textures/internal/spotlight.png", { TextureFormat::SRGB_DXT1, TextureFilter::Bilinear, TextureWrapMode::ClampBorder, TextureWrapMode::ClampBorder });

		// Create HDR lighting framebuffer
		FrameBufferProps lightingProps;
		lightingProps.width = width;
		lightingProps.height = height;
		lightingProps.colorAttachments = { TextureFormat::RG11B10F };

		hdrFrameBuffer = FrameBuffer::Create(lightingProps);

		// Create lighting shader & material
		Ref<Shader> deferredShader = Shader::Create("assets/shaders/internal/DeferredPerLight.yaml");
		deferredMaterial = Material::Create(deferredShader);
		deferredMaterial->SetTexture("u_BRDFLUT", 2, brdfLut);
		deferredMaterial->SetTexture("u_AttenuationLUT", 3, falloffLut);
		deferredMaterial->SetTexture("u_LightCookie", 8, spotlightTexture);
	}

	LightingRenderPass::~LightingRenderPass()
	{
		brdfLut = nullptr;
		falloffLut = nullptr;
		spotlightTexture = nullptr;

		hdrFrameBuffer = nullptr;
		deferredMaterial = nullptr;
	}

	void LightingRenderPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		updateTextures = true;

		hdrFrameBuffer->Resize(width, height);
	}

	void LightingRenderPass::Render(Renderer::SceneData* sceneData, Ref<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (sceneData->wireframe)
		{
			src->Blit(hdrFrameBuffer, true, false);
			return;
		}

		if (updateTextures)
		{
			updateTextures = false;
			deferredMaterial->SetTexture("u_GBuffer0", 4, src->GetColorTexture(0));
			deferredMaterial->SetTexture("u_GBuffer1", 5, src->GetColorTexture(1));
			deferredMaterial->SetTexture("u_GBuffer2", 6, src->GetColorTexture(3));
			deferredMaterial->SetTexture("u_Depth", 7, src->GetDepthTexture());
		}

		if (deferredMaterial->GetTexture("u_IrradianceMap") != sceneData->environment.irradianceMap)
			deferredMaterial->SetTexture("u_IrradianceMap", 0, sceneData->environment.irradianceMap);
		if (deferredMaterial->GetTexture("u_SpecularMap") != sceneData->environment.specularMap)
			deferredMaterial->SetTexture("u_SpecularMap", 1, sceneData->environment.specularMap);

		// Blit depth buffer from gBuffer
		src->Blit(hdrFrameBuffer, false, true);

		// Bind and clear lighting buffer
		hdrFrameBuffer->Bind();
		GL::SetClearColor({ 1.0f, 0.06f, 0.94f, 1.0f });
		GL::Clear(true, false);

		// Ambient lighting and don't write or read depth
		GL::EnableZWriting(false);
		GL::EnableZTesting(false);

		// Directional lights + ambient
		RenderDirectionalLights(sceneData);

		// Render additional lights with additive blend mode
		GL::SetBlendMode(RendererAPI::BlendMode::One, RendererAPI::BlendMode::One, true);
		// TODO: Implement GL_GEQUAL with z-testing
		// GL::enableZTesting(true);

		// Point lights
		RenderPointLights(sceneData);

		// Spot lights
		RenderSpotLights(sceneData);

		// Disable blending
		GL::SetBlendMode(RendererAPI::BlendMode::One, RendererAPI::BlendMode::One, false);
		GL::EnableZTesting(true);

		// Render skybox
		Renderer::DrawSkybox();

		hdrFrameBuffer->Unbind();
	}

	void LightingRenderPass::RenderDirectionalLights(Renderer::SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amountSize = 16;
		uint32_t amount = (uint32_t)sceneData->environment.directionalLights.size();

		if (amount > 0)
		{
			uint32_t lightSize = sizeof(Renderer::DirectionalLight);
			uint32_t bufferSize = amountSize + amount * lightSize;

			if (!sceneData->directionalLightBuffer || sceneData->directionalLightBuffer->GetSize() != bufferSize)
				sceneData->directionalLightBuffer = StorageBuffer::Create(bufferSize);

			sceneData->directionalLightBuffer->SetData(&amount, 0, sizeof(int));
			sceneData->directionalLightBuffer->SetData(&sceneData->environment.directionalLights[0], amountSize, amount * lightSize);
		}
		else if (!sceneData->directionalLightBuffer || sceneData->directionalLightBuffer->GetSize() != amountSize)
		{
			sceneData->directionalLightBuffer = StorageBuffer::Create(amountSize);

			sceneData->directionalLightBuffer->SetData(&amount, 0, sizeof(int));
		}

		deferredMaterial->BindShader("DIRECTIONAL");
		deferredMaterial->Bind();
		sceneData->directionalLightBuffer->Bind(1);

		Renderer::DrawScreenQuad();
	}

	void LightingRenderPass::RenderPointLights(Renderer::SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->environment.pointLights.size();

		if (amount > 0)
		{
			uint32_t lightSize = sizeof(Renderer::PointLight);
			uint32_t bufferSize = amount * lightSize;

			if (!sceneData->pointLightBuffer || sceneData->pointLightBuffer->GetSize() != bufferSize)
				sceneData->pointLightBuffer = StorageBuffer::Create(bufferSize);

			sceneData->pointLightBuffer->SetData(&sceneData->environment.pointLights[0], 0, bufferSize);

			deferredMaterial->BindShader("POINT");
			deferredMaterial->Bind();
			sceneData->pointLightBuffer->Bind(1);

			Renderer::DrawInstancedSphere(amount);
		}
	}

	void LightingRenderPass::RenderSpotLights(Renderer::SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->environment.spotLights.size();

		if (amount > 0)
		{
			uint32_t lightSize = sizeof(Renderer::SpotLight);
			uint32_t bufferSize = amount * lightSize;

			if (!sceneData->spotLightBuffer || sceneData->spotLightBuffer->GetSize() != bufferSize)
				sceneData->spotLightBuffer = StorageBuffer::Create(bufferSize);

			sceneData->spotLightBuffer->SetData(&sceneData->environment.spotLights[0], 0, bufferSize);

			deferredMaterial->BindShader("SPOT");
			deferredMaterial->Bind();
			sceneData->spotLightBuffer->Bind(1);

			Renderer::DrawInstancedPyramid(amount);
		}
	}

	Ref<Texture> LightingRenderPass::LoadOrCreateLUTTexture(const std::string& cachePath, const std::string& shaderPath, TextureFormat format, uint32_t width, uint32_t height)
	{
		if (!std::filesystem::exists(cachePath))
		{
			// Setup LUT shader and framebuffer for capturing
			FrameBufferProps framebufferProps;
			framebufferProps.width = width;
			framebufferProps.height = height;
			framebufferProps.colorAttachments = { format };
			framebufferProps.dontUseDepth = true;
			Ref<FrameBuffer> framebuffer = FrameBuffer::Create(framebufferProps);

			Ref<Shader> shader = Shader::Create(shaderPath);

			framebuffer->Bind();

			shader->Bind("LUT");

			GL::Clear();

			GL::DrawScreenQuad();

			framebuffer->Unbind();

			Ref<Texture> lut = framebuffer->GetColorTexture(0);

			// Save to cache
			uint32_t size = width * height * 4;
			char* pixels = new char[size];
			lut->ReadPixels(pixels);
			std::ofstream stream(cachePath, std::ios::binary);
			stream.write(pixels, size);

			delete[] pixels;

			return lut;
		}
		else
		{
			// Load from cache
			std::ifstream inStream(cachePath, std::ios::binary);
			std::stringstream ss;
			ss << inStream.rdbuf();
			Ref<Texture> lut = Texture2D::Create({ width, height, format, TextureFilter::Bilinear, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });
			lut->SetData((void*)ss.str().c_str(), 512 * 512 * 4); // TODO: Un-hardcode this value

			return lut;
		}
	}
}