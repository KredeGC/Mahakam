#include "mhpch.h"
#include "LightingRenderPass.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Renderer.h"

#include "Mahakam/Renderer/Texture.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Mahakam
{
	void LightingRenderPass::Init(uint32_t width, uint32_t height)
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

		// Create lighting shader
		deferredShader = Shader::Create("assets/shaders/internal/DeferredPerLight.yaml", { "DEBUG" });

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

	LightingRenderPass::~LightingRenderPass()
	{
		brdfLut = nullptr;
		falloffLut = nullptr;
		spotlightTexture = nullptr;

		hdrFrameBuffer = nullptr;
		deferredShader = nullptr;

		shadowFramebuffer = nullptr;
		shadowShader = nullptr;
	}

	void LightingRenderPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		hdrFrameBuffer->Resize(width, height);
	}

	bool LightingRenderPass::Render(SceneData* sceneData, Ref<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (sceneData->wireframe)
		{
			src->Blit(hdrFrameBuffer);
			return false;
		}

		shadowMapOffset = { 0.0f, 0.0f };
		shadowMapMargin = { 0.0f, 0.0f };

		// Render shadow maps
		shadowFramebuffer->Bind();

		GL::Clear(false, true);

		// Directional shadows
		RenderDirectionalShadows(sceneData);

		// Spot shadows
		RenderSpotShadows(sceneData);

		shadowFramebuffer->Unbind();


		// Initialize shader variables
		if (sceneData->gBuffer)
			deferredShader->Bind("DIRECTIONAL", "DEBUG");
		else
			deferredShader->Bind("DIRECTIONAL");
		deferredShader->SetTexture("u_GBuffer0", src->GetColorTexture(0));
		deferredShader->SetTexture("u_GBuffer1", src->GetColorTexture(1));
		deferredShader->SetTexture("u_GBuffer2", src->GetColorTexture(3));
		deferredShader->SetTexture("u_Depth", src->GetDepthTexture());

		deferredShader->SetTexture("u_BRDFLUT", brdfLut);
		deferredShader->SetTexture("u_ShadowMap", shadowFramebuffer->GetDepthTexture());

		deferredShader->SetTexture("u_IrradianceMap", sceneData->environment.irradianceMap);
		deferredShader->SetTexture("u_SpecularMap", sceneData->environment.specularMap);


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
		if (!sceneData->gBuffer)
			Renderer::DrawSkybox();

		hdrFrameBuffer->Unbind();

		return true;
	}

	void LightingRenderPass::RenderShadowGeometry(SceneData* sceneData, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID)
	{
		for (uint64_t drawID : sceneData->renderQueue)
		{
			// Choose a shader
			const uint64_t shaderID = (drawID >> 47ULL) & 0x7FFFULL;
			if (shaderID != *lastShaderID)
			{
				Ref<Shader>& shader = sceneData->shaderIDLookup[shaderID];
				if (shader->HasShaderPass("SHADOW"))
				{
					*lastShaderID = shaderID;
					shader->Bind("SHADOW");
				}
				else if (*lastShaderID != ~0)
				{
					*lastShaderID = ~0;
					shadowShader->Bind("SHADOW");
				}
			}

			// Choose a material
			const uint64_t materialID = (drawID >> 32ULL) & 0x7FFFULL;
			Ref<Material>& material = sceneData->materialIDLookup[materialID];
			if (materialID != *lastMaterialID && *lastShaderID != ~0)
			{
				*lastMaterialID = materialID;
				material->Bind();
			}

			// Choose a mesh
			const uint64_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
			Ref<Mesh>& mesh = sceneData->meshIDLookup[meshID];
			if (meshID != *lastMeshID)
			{
				*lastMeshID = meshID;
				mesh->Bind();
			}

			// Choose a transform
			const uint64_t transformID = drawID & 0xFFFFULL;
			glm::mat4& transform = sceneData->transformIDLookup[transformID];

			// Render to depth map
			material->SetTransform(transform);

			Renderer::AddPerformanceResult(mesh->GetVertexCount(), mesh->GetIndexCount());

			GL::DrawIndexed(mesh->GetIndexCount());
		}
	}

	void LightingRenderPass::RenderDirectionalShadows(SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->environment.directionalLights.size();
		if (amount > 0)
		{
			// Choose size and offset of shadow texture
			constexpr uint32_t size = 2048;
			constexpr uint32_t ratio = shadowMapSize / size;

			// Setup and bind default shadow shader
			uint64_t lastShaderID = ~0;
			uint64_t lastMaterialID = ~0;
			uint64_t lastMeshID = ~0;
			shadowShader->Bind("SHADOW");

			for (uint32_t i = 0; i < amount; i++)
			{
				DirectionalLight& light = sceneData->environment.directionalLights[i];

				// Check if light is disabled
				if (light.offset.z == 0.0f)
					continue;

				if (shadowMapOffset.x + size > shadowMapSize)
				{
					shadowMapOffset.y += size;

					if (shadowMapOffset.y > shadowMapMargin.y)
						shadowMapMargin.x = 0;

					shadowMapOffset.x = shadowMapMargin.x;
				}

				uint32_t x = shadowMapOffset.x;
				uint32_t y = shadowMapOffset.y;

				/*uint32_t x = shadowOffset % ratio;
				uint32_t y = shadowOffset / ratio;*/

				GL::SetViewport(x, y, size, size);

				light.offset = { x, y, 1.0f / (float)ratio, light.offset.w };

				// Avoid edge swimming by snapping to nearest texel
				float texelSize = 1.0f / (float)size;

				light.worldToLight[3][0] -= glm::mod(light.worldToLight[3][0], 2.0f * texelSize);
				light.worldToLight[3][1] -= glm::mod(light.worldToLight[3][1], 2.0f * texelSize);
				light.worldToLight[3][2] -= glm::mod(light.worldToLight[3][2], 2.0f * texelSize);

				// Bind worldToLight matrix
				shadowMatrixBuffer->SetData(&light.worldToLight, 0, sizeof(glm::mat4));
				shadowMatrixBuffer->Bind(1);

				shadowMapOffset.x += size;

				//shadowOffset++;

				// Render all objects in queue
				RenderShadowGeometry(sceneData, &lastShaderID, &lastMaterialID, &lastMeshID);
			}

			shadowMapMargin.x = shadowMapOffset.x;
			shadowMapMargin.y = shadowMapOffset.y + size;

			/*shadowMapOffset.x = 0;
			shadowMapOffset.y += shadowMaxSize;*/
		}
	}

	void LightingRenderPass::RenderSpotShadows(SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->environment.spotLights.size();
		if (amount > 0)
		{
			// Choose size and offset of shadow texture
			constexpr uint32_t size = 512;
			constexpr uint32_t ratio = shadowMapSize / size;

			// Setup and bind default shadow shader
			uint64_t lastShaderID = ~0;
			uint64_t lastMaterialID = ~0;
			uint64_t lastMeshID = ~0;
			shadowShader->Bind("SHADOW");

			for (uint32_t i = 0; i < amount; i++)
			{
				SpotLight& light = sceneData->environment.spotLights[i];

				// Check if light is disabled
				if (light.offset.z == 0.0f)
					continue;

				if (shadowMapOffset.x + size > shadowMapSize)
				{
					shadowMapOffset.y += size;

					if (shadowMapOffset.y >= shadowMapMargin.y)
						shadowMapMargin.x = 0;

					shadowMapOffset.x = shadowMapMargin.x;
				}

				uint32_t x = shadowMapOffset.x;
				uint32_t y = shadowMapOffset.y;

				GL::SetViewport(x, y, size, size);

				light.offset = { x / (ratio * (float)size), y / (ratio * (float)size), 1.0f / (float)ratio, light.offset.w };

				// Bind worldToLight matrix
				shadowMatrixBuffer->SetData(&light.worldToLight, 0, sizeof(glm::mat4));
				shadowMatrixBuffer->Bind(1);

				shadowMapOffset.x += size;

				// Render all objects in queue
				RenderShadowGeometry(sceneData, &lastShaderID, &lastMaterialID, &lastMeshID);
			}
		}
	}

	void LightingRenderPass::RenderDirectionalLights(SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amountSize = 16;
		uint32_t amount = (uint32_t)sceneData->environment.directionalLights.size();

		if (amount > 0)
		{
			uint32_t lightSize = sizeof(DirectionalLight);
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

		sceneData->directionalLightBuffer->Bind(1);

		Renderer::DrawScreenQuad();
	}

	void LightingRenderPass::RenderPointLights(SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->environment.pointLights.size();

		if (amount > 0)
		{
			uint32_t lightSize = sizeof(PointLight);
			uint32_t bufferSize = amount * lightSize;

			if (!sceneData->pointLightBuffer || sceneData->pointLightBuffer->GetSize() != bufferSize)
				sceneData->pointLightBuffer = StorageBuffer::Create(bufferSize);

			sceneData->pointLightBuffer->SetData(&sceneData->environment.pointLights[0], 0, bufferSize);

			if (sceneData->gBuffer)
				deferredShader->Bind("POINT", "DEBUG");
			else
				deferredShader->Bind("POINT");
			deferredShader->SetTexture("u_AttenuationLUT", falloffLut);
			sceneData->pointLightBuffer->Bind(1);

			Renderer::DrawInstancedSphere(amount);
		}
	}

	void LightingRenderPass::RenderSpotLights(SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->environment.spotLights.size();

		if (amount > 0)
		{
			uint32_t lightSize = sizeof(SpotLight);
			uint32_t bufferSize = amount * lightSize;

			if (!sceneData->spotLightBuffer || sceneData->spotLightBuffer->GetSize() != bufferSize)
				sceneData->spotLightBuffer = StorageBuffer::Create(bufferSize);

			sceneData->spotLightBuffer->SetData(&sceneData->environment.spotLights[0], 0, bufferSize);

			if (sceneData->gBuffer)
				deferredShader->Bind("SPOT", "DEBUG");
			else
				deferredShader->Bind("SPOT");
			deferredShader->SetTexture("u_AttenuationLUT", falloffLut);
			deferredShader->SetTexture("u_LightCookie", spotlightTexture);
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