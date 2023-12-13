#include "Mahakam/mhpch.h"
#include "LightingRenderPass.h"

#include "Mahakam/Core/Frustum.h"
#include "Mahakam/Core/FileUtility.h"

#include "Mahakam/Math/Bounds.h"

#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/RenderData.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/Shader.h"
#include "Mahakam/Renderer/Texture.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Mahakam
{
	bool LightingRenderPass::Init(uint32_t width, uint32_t height)
	{
		if (RenderPass::Init(width, height))
			return true;

		MH_PROFILE_RENDERING_FUNCTION();

		SetupBRDF();
		SetupFrameBuffer(width, height);
		SetupShaders();

		// Create shadow map
		FrameBufferProps shadowProps;
		shadowProps.Width = s_ShadowMapSize;
		shadowProps.Height = s_ShadowMapSize;
		shadowProps.DepthAttachment = { TextureFormat::Depth24, TextureFilter::Point };
		m_ShadowFramebuffer = FrameBuffer::Create(shadowProps);

		Renderer::AddFrameBuffer("Shadow Atlas", m_ShadowFramebuffer);

		m_ShadowMatrixBuffer = UniformBuffer::Create(sizeof(glm::mat4));

		return true;
	}

	LightingRenderPass::~LightingRenderPass()
	{
		MH_PROFILE_FUNCTION();

		m_BRDFLut = nullptr;
		m_FalloffLut = nullptr;
		m_SpotlightTexture = nullptr;

		m_HDRFrameBuffer = nullptr;
		m_DeferredShader = nullptr;

		m_ShadowFramebuffer = nullptr;
		m_ShadowShader = nullptr;
	}

	void LightingRenderPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		m_HDRFrameBuffer->Resize(width, height);
	}

	bool LightingRenderPass::Render(SceneData* sceneData, const Asset<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (sceneData->Wireframe)
			return false;

		// Create view projection frustum
		Frustum cameraFrustum(sceneData->CameraMatrix.u_m4_VP);

		// Render shadow maps
		RenderShadowMaps(sceneData, cameraFrustum);

		// Initialize deferred shader variables
		SetupTextures(sceneData, src);

		// Render lights to final buffer
		RenderLighting(sceneData, src);

		return true;
	}

	void LightingRenderPass::SetupBRDF()
	{
		MH_PROFILE_RENDERING_FUNCTION();

		// Create BRDF and falloff maps
		FileUtility::CreateDirectories("cache/internal/");

		m_BRDFLut = LoadOrCreateLUTTexture("cache/internal/BRDF.dat", "internal/shaders/builtin/BRDF.shader", TextureFormat::RG16F, 512, 512);
		m_FalloffLut = LoadOrCreateLUTTexture("cache/internal/Falloff.dat", "internal/shaders/builtin/Falloff.shader", TextureFormat::R8, 16, 16);

		m_SpotlightTexture = Texture2D::Create("internal/textures/spotlight.png", { TextureFormat::SRGB_DXT1, TextureFilter::Bilinear, TextureWrapMode::ClampBorder, TextureWrapMode::ClampBorder });
	}

	void LightingRenderPass::SetupFrameBuffer(uint32_t width, uint32_t height)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		// Create HDR lighting framebuffer
		FrameBufferProps lightingProps;
		lightingProps.Width = width;
		lightingProps.Height = height;
		lightingProps.ColorAttachments = TrivialVector<FrameBufferAttachmentProps>{ TextureFormat::RG11B10F };
		lightingProps.DepthAttachment = { TextureFormat::Depth24 };

		m_HDRFrameBuffer = FrameBuffer::Create(lightingProps);

		Renderer::AddFrameBuffer("Lighting", m_HDRFrameBuffer);
	}

	void LightingRenderPass::SetupShaders()
	{
		MH_PROFILE_RENDERING_FUNCTION();

		// Create lighting shader
		m_DeferredShader = Shader::Create("internal/shaders/builtin/DeferredPerLight.shader");

		// Create default shadow shader
		m_ShadowShader = Shader::Create("internal/shaders/default/Shadow.shader");
	}

	void LightingRenderPass::RenderShadowMaps(SceneData* sceneData, const Frustum& frustum)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		m_ShadowMapOffset = { 0.0f, 0.0f };
		m_ShadowMapMargin = { 0.0f, 0.0f };

		m_ShadowFramebuffer->Bind();

		// Setup and bind shadow shader
		uint64_t lastShaderID = ~0;
		uint64_t lastMaterialID = ~0;
		uint64_t lastMeshID = ~0;
		m_ShadowShader->Bind("SHADOW");

		// Bind worldToLight matrix
		m_ShadowMatrixBuffer->Bind(1);

		// Directional shadows
		RenderDirectionalShadows(sceneData, &lastShaderID, &lastMaterialID, &lastMeshID);

		// Spot shadows
		RenderSpotShadows(sceneData, frustum, &lastShaderID, &lastMaterialID, &lastMeshID);

		m_ShadowFramebuffer->Unbind();
	}

	void LightingRenderPass::SetupTextures(SceneData* sceneData, const Asset<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		m_DeferredShader->Bind("DIRECTIONAL");
		m_DeferredShader->SetTexture("u_GBuffer0", *src->GetColorTexture(0));
		m_DeferredShader->SetTexture("u_GBuffer1", *src->GetColorTexture(1));
		m_DeferredShader->SetTexture("u_GBuffer2", *src->GetColorTexture(2));
		m_DeferredShader->SetTexture("u_GBuffer3", *src->GetColorTexture(3));
		m_DeferredShader->SetTexture("u_Depth", *src->GetDepthTexture());

		m_DeferredShader->SetTexture("u_BRDFLUT", *m_BRDFLut);
		m_DeferredShader->SetTexture("u_ShadowMap", *m_ShadowFramebuffer->GetDepthTexture());

		if (sceneData->Environment.IrradianceMap)
			m_DeferredShader->SetTexture("u_IrradianceMap", *sceneData->Environment.IrradianceMap);

		if (sceneData->Environment.SpecularMap)
			m_DeferredShader->SetTexture("u_SpecularMap", *sceneData->Environment.SpecularMap);
	}

	void LightingRenderPass::RenderLighting(SceneData* sceneData, const Asset<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		GL::SetViewport(0, 0, m_HDRFrameBuffer->GetSpecification().Width, m_HDRFrameBuffer->GetSpecification().Height);

		// Blit depth buffer from gBuffer
		src->Blit(m_HDRFrameBuffer, false, true);

		// Bind and clear lighting buffer
		m_HDRFrameBuffer->Bind();
		GL::SetClearColor({ 1.0f, 0.06f, 0.94f, 1.0f });
		GL::Clear(true, false);

		// Don't write or read depth
		GL::EnableZWriting(false);
		GL::SetZTesting(RendererAPI::DepthMode::Always);

		// Directional lights + ambient
		RenderDirectionalLights(sceneData);

		// Render additional lights with additive blend mode
		GL::SetBlendMode(RendererAPI::BlendMode::One, RendererAPI::BlendMode::One, true);
		GL::SetZTesting(RendererAPI::DepthMode::GEqual); // TODO: Remove when implementing volumetric spotlights

		// Point lights
		RenderPointLights(sceneData);

		// Spot lights
		RenderSpotLights(sceneData);

		// Disable blending
		GL::SetBlendMode(RendererAPI::BlendMode::One, RendererAPI::BlendMode::One, false);
		GL::SetZTesting(RendererAPI::DepthMode::LEqual);

		m_HDRFrameBuffer->Unbind();
	}

	uint64_t LightingRenderPass::PrePassShadowGeometry(SceneData* sceneData, const Frustum& frustum)
	{
		MH_PROFILE_FUNCTION();

		uint64_t hash = 2166136261ULL;

		for (uint64_t drawID : sceneData->RenderQueue)
		{
			// Choose a mesh
			const uint64_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
			Ref<SubMesh>& mesh = sceneData->MeshIDLookup[meshID];

			// Choose a transform
			const uint64_t transformID = drawID & 0xFFFFULL;
			const glm::mat4& transform = sceneData->TransformIDLookup[transformID];

			// Perform AABB test
			const Bounds transformedBounds = Bounds::TransformBounds(mesh->GetBounds(), transform);

			if (frustum.IsBoxVisible(transformedBounds.Min, transformedBounds.Max))
			{
				// Choose a shader
				const uint64_t shaderID = (drawID >> 47ULL) & 0x7FFFULL;
				Asset<Shader>& shader = sceneData->ShaderIDLookup[shaderID];

				// Choose a material
				const uint64_t materialID = (drawID >> 32ULL) & 0x7FFFULL;
				Asset<Material>& material = sceneData->MaterialIDLookup[materialID];

				// Hash shader
				const uint8_t* shaderPtr = reinterpret_cast<const uint8_t*>(shader.get());
				for (uint64_t i = 0; i < sizeof(Shader*); ++i)
					hash = (hash * 16777619ULL) ^ *(shaderPtr + i);

				// Hash material
				uint64_t materialHash = material->Hash();
				const uint8_t* materialPtr = reinterpret_cast<const uint8_t*>(&materialHash);
				for (uint64_t i = 0; i < sizeof(uint64_t); ++i)
					hash = (hash * 16777619ULL) ^ *(materialPtr + i);

				// Hash mesh
				const uint8_t* meshPtr = reinterpret_cast<const uint8_t*>(mesh.get());
				for (uint64_t i = 0; i < sizeof(SubMesh*); ++i)
					hash = (hash * 16777619ULL) ^ *(meshPtr + i);

				// Hash transform
				const uint8_t* transformPtr = reinterpret_cast<const uint8_t*>(glm::value_ptr(transform));
				for (uint64_t i = 0; i < sizeof(glm::mat4); ++i)
					hash = (hash * 16777619ULL) ^ *(transformPtr + i);

				// Add to render queue
				m_RenderQueue.push_back(drawID);
			}
		}

		return hash;
	}

	void LightingRenderPass::RenderShadowGeometry(SceneData* sceneData, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		GL::Clear(false, true);

		for (uint64_t drawID : m_RenderQueue)
		{
			// Choose a mesh
			const uint64_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
			Ref<SubMesh>& mesh = sceneData->MeshIDLookup[meshID];

			// Choose a transform
			const uint64_t transformID = drawID & 0xFFFFULL;
			glm::mat4& transform = sceneData->TransformIDLookup[transformID];
			
			// Choose and bind shader
			const uint64_t shaderID = (drawID >> 47ULL) & 0x7FFFULL;
			Asset<Shader>& shader = sceneData->ShaderIDLookup[shaderID];
			if (shaderID != *lastShaderID)
			{
				if (shader->HasShaderPass("SHADOW"))
				{
					*lastShaderID = shaderID;
					shader->Bind("SHADOW");
				}
				else if (*lastShaderID != ~0)
				{
					*lastShaderID = ~0;
					m_ShadowShader->Bind("SHADOW");
				}
			}

			// Choose and bind material
			const uint64_t materialID = (drawID >> 32ULL) & 0x7FFFULL;
			Asset<Material>& material = sceneData->MaterialIDLookup[materialID];
			if (materialID != *lastMaterialID && *lastShaderID != ~0)
			{
				*lastMaterialID = materialID;
				material->Bind(*sceneData->UniformValueBuffer);
			}

			// Bind mesh
			if (meshID != *lastMeshID)
			{
				*lastMeshID = meshID;
				mesh->Bind();
			}

			// Render to depth map
			sceneData->CameraBuffer->SetData(&transform, 0, sizeof(glm::mat4));

			Renderer::AddPerformanceResult(mesh->GetVertexCount(), mesh->GetIndexCount());

			GL::DrawIndexed(mesh->GetIndexCount());
		}
	}

	void LightingRenderPass::RenderDirectionalShadows(SceneData* sceneData, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->Environment.DirectionalLights.size();
		if (amount > 0)
		{
			// Choose size and offset of shadow texture
			constexpr uint32_t size = s_DirectionalShadowSize;
			constexpr uint32_t ratio = s_ShadowMapSize / size;
			constexpr float texelSize = 2.0f / (float)size;

			for (uint32_t i = 0; i < amount; i++)
			{
				DirectionalLight& light = sceneData->Environment.DirectionalLights[i];

				// Check if light is disabled
				if (light.offset.z == 0.0f)
					continue;

				// Update current shadow map offset
				if (m_ShadowMapOffset.x + size > s_ShadowMapSize)
				{
					m_ShadowMapOffset.y += size;

					if (m_ShadowMapOffset.y > m_ShadowMapMargin.y)
						m_ShadowMapMargin.x = 0;

					m_ShadowMapOffset.x = m_ShadowMapMargin.x;
				}

				const glm::ivec2 currentOffset = m_ShadowMapOffset;

				m_ShadowMapOffset.x += size;

				light.offset = { currentOffset.x / (ratio * (float)size), currentOffset.y / (ratio * (float)size), 1.0f / (float)ratio, light.offset.w };

				// Avoid edge swimming by snapping to nearest texel
				light.worldToLight[3][0] -= glm::mod(light.worldToLight[3][0], texelSize);
				light.worldToLight[3][1] -= glm::mod(light.worldToLight[3][1], texelSize);
				light.worldToLight[3][2] -= glm::mod(light.worldToLight[3][2], texelSize);

				// Calculate hash
				Frustum frustum(light.worldToLight);

				m_RenderQueue.clear();

				uint64_t hash = light.Hash();
				uint64_t frustumHash = PrePassShadowGeometry(sceneData, frustum);

				const uint8_t* p = reinterpret_cast<const uint8_t*>(&frustumHash);
				for (uint64_t i = 0; i < sizeof(uint64_t); ++i)
					hash = (hash * 16777619ULL) ^ p[i];

				auto iter = m_LightHashes.find(currentOffset);
				if (iter != m_LightHashes.end() && iter->second == hash) // If the hashes match, skip rendering
					continue;
				m_LightHashes[currentOffset] = hash;

				// Set light data in buffer
				m_ShadowMatrixBuffer->SetData(&light.worldToLight, 0, sizeof(glm::mat4));

				// Render all objects in queue
				GL::SetViewport(currentOffset.x, currentOffset.y, size, size, true);

				RenderShadowGeometry(sceneData, lastShaderID, lastMaterialID, lastMeshID);
			}

			m_ShadowMapMargin.x = m_ShadowMapOffset.x;
			m_ShadowMapMargin.y = m_ShadowMapOffset.y + size;
		}
	}

	void LightingRenderPass::RenderSpotShadows(SceneData* sceneData, const Frustum& cameraFrustum, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->Environment.SpotLights.size();
		if (amount > 0)
		{
			// Choose size and offset of shadow texture
			constexpr uint32_t size = s_SpotShadowSize;
			constexpr uint32_t ratio = s_ShadowMapSize / size;

			for (uint32_t i = 0; i < amount; i++)
			{
				SpotLight& light = sceneData->Environment.SpotLights[i];

				// Check if light is disabled
				if (light.offset.z == 0.0f)
					continue;

				// Perform AABB test
				Bounds transformedBounds = Bounds::TransformBounds(GL::GetInvertedPyramid()->GetBounds(), light.objectToWorld);

				if (!cameraFrustum.IsBoxVisible(transformedBounds.Min, transformedBounds.Max))
					continue;

				// Update current shadow map offset
				if (m_ShadowMapOffset.x + size > s_ShadowMapSize)
				{
					m_ShadowMapOffset.y += size;

					if (m_ShadowMapOffset.y > m_ShadowMapMargin.y)
						m_ShadowMapMargin.x = 0;

					m_ShadowMapOffset.x = m_ShadowMapMargin.x;
				}

				const glm::ivec2 currentOffset = m_ShadowMapOffset;

				m_ShadowMapOffset.x += size;

				light.offset = { currentOffset.x / (ratio * (float)size), currentOffset.y / (ratio * (float)size), 1.0f / (float)ratio, light.offset.w };

				// Calculate hash
				Frustum frustum(light.worldToLight);

				m_RenderQueue.clear();

				uint64_t hash = light.Hash();

				uint64_t frustumHash = PrePassShadowGeometry(sceneData, frustum);

				const uint8_t* p = reinterpret_cast<const uint8_t*>(&frustumHash);
				for (uint64_t i = 0; i < sizeof(uint64_t); ++i)
					hash = (hash * 16777619ULL) ^ p[i];

				auto iter = m_LightHashes.find(currentOffset);
				if (iter != m_LightHashes.end() && iter->second == hash) // If the hashes match, skip rendering
					continue;
				m_LightHashes[currentOffset] = hash;

				// Set light data in buffer
				m_ShadowMatrixBuffer->SetData(&light.worldToLight, 0, sizeof(glm::mat4));

				// Render all objects in queue
				GL::SetViewport(currentOffset.x, currentOffset.y, size, size, true);

				RenderShadowGeometry(sceneData, lastShaderID, lastMaterialID, lastMeshID);
			}
		}
	}

	void LightingRenderPass::RenderDirectionalLights(SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amountSize = 16;
		uint32_t amount = (uint32_t)sceneData->Environment.DirectionalLights.size();

		if (sceneData->DirectionalLightBuffer)
			sceneData->DirectionalLightBuffer->Bind(1);

		if (amount > 0)
		{
			uint32_t lightSize = sizeof(DirectionalLight);
			uint32_t bufferSize = amountSize + amount * lightSize;

			if (!sceneData->DirectionalLightBuffer || sceneData->DirectionalLightBuffer->GetSize() != bufferSize)
				sceneData->DirectionalLightBuffer = StorageBuffer::Create(bufferSize);

			sceneData->DirectionalLightBuffer->SetData(&amount, 0, sizeof(int));
			sceneData->DirectionalLightBuffer->SetData(sceneData->Environment.DirectionalLights.data(), amountSize, amount * lightSize);
		}
		else if (!sceneData->DirectionalLightBuffer || sceneData->DirectionalLightBuffer->GetSize() != amountSize)
		{
			sceneData->DirectionalLightBuffer = StorageBuffer::Create(amountSize);

			sceneData->DirectionalLightBuffer->SetData(&amount, 0, sizeof(int));
		}

		Renderer::DrawScreenQuad();
	}

	void LightingRenderPass::RenderPointLights(SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->Environment.PointLights.size();

		if (amount > 0)
		{
			uint32_t lightSize = sizeof(PointLight);
			uint32_t bufferSize = amount * lightSize;

			if (!sceneData->PointLightBuffer || sceneData->PointLightBuffer->GetSize() != bufferSize)
				sceneData->PointLightBuffer = StorageBuffer::Create(bufferSize);

			sceneData->PointLightBuffer->Bind(1);
			sceneData->PointLightBuffer->SetData(sceneData->Environment.PointLights.data(), 0, bufferSize);

			m_DeferredShader->Bind("POINT");
			m_DeferredShader->SetTexture("u_AttenuationLUT", *m_FalloffLut);

			Renderer::DrawInstancedSphere(amount);
		}
	}

	void LightingRenderPass::RenderSpotLights(SceneData* sceneData)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		uint32_t amount = (uint32_t)sceneData->Environment.SpotLights.size();

		if (amount > 0)
		{
			uint32_t lightSize = sizeof(SpotLight);
			uint32_t bufferSize = amount * lightSize;

			if (!sceneData->SpotLightBuffer || sceneData->SpotLightBuffer->GetSize() != bufferSize)
				sceneData->SpotLightBuffer = StorageBuffer::Create(bufferSize);

			sceneData->SpotLightBuffer->Bind(1);
			sceneData->SpotLightBuffer->SetData(sceneData->Environment.SpotLights.data(), 0, bufferSize);

			m_DeferredShader->Bind("SPOT");
			m_DeferredShader->SetTexture("u_AttenuationLUT", *m_FalloffLut);
			m_DeferredShader->SetTexture("u_LightCookie", *m_SpotlightTexture);

			Renderer::DrawInstancedPyramid(amount);
		}
	}

	Asset<Texture> LightingRenderPass::LoadOrCreateLUTTexture(const std::string& cachePath, const std::string& shaderPath, TextureFormat format, uint32_t width, uint32_t height)
	{
		if (!FileUtility::Exists(cachePath))
		{
			// Setup LUT shader and framebuffer for capturing
			FrameBufferProps framebufferProps;
			framebufferProps.Width = width;
			framebufferProps.Height = height;
			framebufferProps.ColorAttachments = TrivialVector<FrameBufferAttachmentProps>{ format };
			framebufferProps.DontUseDepth = true;
			Asset<FrameBuffer> framebuffer = FrameBuffer::Create(framebufferProps);

			Asset<Shader> shader = Shader::Create(shaderPath);

			framebuffer->Bind();

			shader->Bind("LUT");

			GL::Clear();

			GL::DrawScreenQuad();

			framebuffer->Unbind();

			Asset<Texture> lut = framebuffer->GetColorTexture(0);

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
			Asset<Texture> lut = Texture2D::Create({ width, height, format, TextureFilter::Bilinear, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });
			lut->SetData((void*)ss.str().c_str(), 512 * 512 * 4); // TODO: Un-hardcode this value

			return lut;
		}
	}
}