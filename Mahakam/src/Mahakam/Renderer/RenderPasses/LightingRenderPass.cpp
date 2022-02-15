#include "mhpch.h"
#include "LightingRenderPass.h"

#include "Mahakam/Core/Frustum.h"

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

		// Create view projection frustum
		Frustum cameraFrustum(sceneData->cameraData.u_m4_VP);

		// Render shadow maps
		{
			MH_PROFILE_RENDERING_SCOPE("Mahakam::LightingRenderPass::Render - Shadow maps");

			shadowMapOffset = { 0.0f, 0.0f };
			shadowMapMargin = { 0.0f, 0.0f };

			shadowFramebuffer->Bind();

			// Setup and bind shadow shader
			uint64_t lastShaderID = ~0;
			uint64_t lastMaterialID = ~0;
			uint64_t lastMeshID = ~0;
			shadowShader->Bind("SHADOW");

			// Directional shadows
			RenderDirectionalShadows(sceneData, &lastShaderID, &lastMaterialID, &lastMeshID);

			// Spot shadows
			RenderSpotShadows(sceneData, cameraFrustum, &lastShaderID, &lastMaterialID, &lastMeshID);

			shadowFramebuffer->Unbind();
		}


		// Initialize deferred shader variables
		{
			MH_PROFILE_RENDERING_SCOPE("Mahakam::LightingRenderPass::Render - Setup");

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
		}


		// Render lights to final buffer
		{
			MH_PROFILE_RENDERING_SCOPE("Mahakam::LightingRenderPass::Render - Deferred lighting");

			GL::SetViewport(0, 0, hdrFrameBuffer->GetSpecification().width, hdrFrameBuffer->GetSpecification().height);

			// Blit depth buffer from gBuffer
			src->Blit(hdrFrameBuffer, false, true);

			// Bind and clear lighting buffer
			hdrFrameBuffer->Bind();
			GL::SetClearColor({ 1.0f, 0.06f, 0.94f, 1.0f });
			GL::Clear(true, false);

			// Don't write or read depth
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
		}

		return true;
	}

	uint64_t LightingRenderPass::PrePassShadowGeometry(SceneData* sceneData, const Frustum& frustum, std::vector<uint64_t>& renderQueue)
	{
		MH_PROFILE_FUNCTION();

		uint64_t hash = 2166136261ULL;

		for (uint64_t drawID : sceneData->renderQueue)
		{
			// Choose a shader
			const uint64_t shaderID = (drawID >> 47ULL) & 0x7FFFULL;
			const Ref<Shader>& shader = sceneData->shaderIDLookup[shaderID];

			// Choose a material
			const uint64_t materialID = (drawID >> 32ULL) & 0x7FFFULL;
			const Ref<Material>& material = sceneData->materialIDLookup[materialID];

			// Choose a mesh
			const uint64_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
			const Ref<Mesh>& mesh = sceneData->meshIDLookup[meshID];

			// Choose a transform
			const uint64_t transformID = drawID & 0xFFFFULL;
			const glm::mat4& transform = sceneData->transformIDLookup[transformID];

			// Perform AABB test
			const Mesh::Bounds transformedBounds = Mesh::TransformBounds(mesh->GetBounds(), transform);

			if (frustum.IsBoxVisible(transformedBounds.min, transformedBounds.max))
			{
				// Hash shader
				unsigned char shaderBytes[sizeof(Shader*)];
				memcpy(shaderBytes, shader.get(), sizeof(Shader*));
				for (uint64_t i = 0; i < sizeof(Shader*); ++i)
					hash = (hash * 16777619ULL) ^ shaderBytes[i];

				// Hash material
				uint64_t materialHash = material->Hash();
				unsigned char materialBytes[sizeof(uint64_t)];
				memcpy(materialBytes, &materialHash, sizeof(uint64_t));
				for (uint64_t i = 0; i < sizeof(uint64_t); ++i)
					hash = (hash * 16777619ULL) ^ materialBytes[i];

				// Hash mesh
				unsigned char meshBytes[sizeof(Mesh*)];
				memcpy(meshBytes, mesh.get(), sizeof(Mesh*));
				for (uint64_t i = 0; i < sizeof(Mesh*); ++i)
					hash = (hash * 16777619ULL) ^ meshBytes[i];

				// Hash transform
				unsigned char transformBytes[sizeof(glm::mat4)];
				memcpy(transformBytes, &transform, sizeof(glm::mat4));
				for (uint64_t i = 0; i < sizeof(glm::mat4); ++i)
					hash = (hash * 16777619ULL) ^ transformBytes[i];

				// Add to render queue
				renderQueue.push_back(drawID);
			}
		}

		return hash;
	}

	void LightingRenderPass::RenderShadowGeometry(SceneData* sceneData, const std::vector<uint64_t>& renderQueue, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		GL::Clear(false, true);

		for (uint64_t drawID : renderQueue)
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

			// Perform AABB test
			Mesh::Bounds transformedBounds = Mesh::TransformBounds(mesh->GetBounds(), transform);

			// Render to depth map
			material->SetTransform(transform);

			Renderer::AddPerformanceResult(mesh->GetVertexCount(), mesh->GetIndexCount());

			GL::DrawIndexed(mesh->GetIndexCount());
		}
	}

	void LightingRenderPass::RenderDirectionalShadows(SceneData* sceneData, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->environment.directionalLights.size();
		if (amount > 0)
		{
			// Choose size and offset of shadow texture
			constexpr uint32_t size = 2048;
			constexpr uint32_t ratio = shadowMapSize / size;
			constexpr float texelSize = 2.0f / (float)size;

			for (uint32_t i = 0; i < amount; i++)
			{
				DirectionalLight& light = sceneData->environment.directionalLights[i];

				// Check if light is disabled
				if (light.offset.z == 0.0f)
					continue;

				// Update current shadow map offset
				if (shadowMapOffset.x + size > shadowMapSize)
				{
					shadowMapOffset.y += size;

					if (shadowMapOffset.y > shadowMapMargin.y)
						shadowMapMargin.x = 0;

					shadowMapOffset.x = shadowMapMargin.x;
				}

				const glm::ivec2 currentOffset = shadowMapOffset;

				shadowMapOffset.x += size;

				light.offset = { currentOffset.x / (ratio * (float)size), currentOffset.y / (ratio * (float)size), 1.0f / (float)ratio, light.offset.w };

				// Avoid edge swimming by snapping to nearest texel
				light.worldToLight[3][0] -= glm::mod(light.worldToLight[3][0], texelSize);
				light.worldToLight[3][1] -= glm::mod(light.worldToLight[3][1], texelSize);
				light.worldToLight[3][2] -= glm::mod(light.worldToLight[3][2], texelSize);

				// Calculate hash
				Frustum frustum(light.worldToLight);

				std::vector<uint64_t> renderQueue;

				uint64_t hash = light.Hash();
				uint64_t frustumHash = PrePassShadowGeometry(sceneData, frustum, renderQueue);

				const unsigned char* p = reinterpret_cast<const unsigned char*>(&frustumHash);
				for (uint64_t i = 0; i < sizeof(uint64_t); ++i)
					hash = (hash * 16777619ULL) ^ p[i];

				auto iter = lightHashes.find(currentOffset);
				if (iter != lightHashes.end() && iter->second == hash) // If the hashes match, skip rendering
					continue;
				lightHashes[currentOffset] = hash;

				// Bind worldToLight matrix
				shadowMatrixBuffer->SetData(&light.worldToLight, 0, sizeof(glm::mat4));
				shadowMatrixBuffer->Bind(1);

				// Render all objects in queue
				GL::SetViewport(currentOffset.x, currentOffset.y, size, size, true);

				RenderShadowGeometry(sceneData, renderQueue, lastShaderID, lastMaterialID, lastMeshID);
			}

			shadowMapMargin.x = shadowMapOffset.x;
			shadowMapMargin.y = shadowMapOffset.y + size;
		}
	}

	void LightingRenderPass::RenderSpotShadows(SceneData* sceneData, const Frustum& cameraFrustum, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->environment.spotLights.size();
		if (amount > 0)
		{
			// Choose size and offset of shadow texture
			constexpr uint32_t size = 512;
			constexpr uint32_t ratio = shadowMapSize / size;

			for (uint32_t i = 0; i < amount; i++)
			{
				SpotLight& light = sceneData->environment.spotLights[i];

				// Check if light is disabled
				if (light.offset.z == 0.0f)
					continue;

				// Perform AABB test
				Mesh::Bounds transformedBounds = Mesh::TransformBounds(GL::GetInvertedPyramid()->GetBounds(), light.objectToWorld);

				if (!cameraFrustum.IsBoxVisible(transformedBounds.min, transformedBounds.max))
					continue;

				// Update current shadow map offset
				if (shadowMapOffset.x + size > shadowMapSize)
				{
					shadowMapOffset.y += size;

					if (shadowMapOffset.y > shadowMapMargin.y)
						shadowMapMargin.x = 0;

					shadowMapOffset.x = shadowMapMargin.x;
				}

				const glm::ivec2 currentOffset = shadowMapOffset;

				shadowMapOffset.x += size;

				light.offset = { currentOffset.x / (ratio * (float)size), currentOffset.y / (ratio * (float)size), 1.0f / (float)ratio, light.offset.w };

				// TODO: Calculate a hash based on objects in frustum
				Frustum frustum(light.worldToLight);

				std::vector<uint64_t> renderQueue;

				uint64_t hash = light.Hash();

				uint64_t frustumHash = PrePassShadowGeometry(sceneData, frustum, renderQueue);

				const unsigned char* p = reinterpret_cast<const unsigned char*>(&frustumHash);
				for (uint64_t i = 0; i < sizeof(uint64_t); ++i)
					hash = (hash * 16777619ULL) ^ p[i];

				auto iter = lightHashes.find(currentOffset);
				if (iter != lightHashes.end() && iter->second == hash) // If the hashes match, skip rendering
					continue;
				lightHashes[currentOffset] = hash;

				// Bind worldToLight matrix
				shadowMatrixBuffer->SetData(&light.worldToLight, 0, sizeof(glm::mat4));
				shadowMatrixBuffer->Bind(1);

				// Render all objects in queue
				GL::SetViewport(currentOffset.x, currentOffset.y, size, size, true);

				RenderShadowGeometry(sceneData, renderQueue, lastShaderID, lastMaterialID, lastMeshID);
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