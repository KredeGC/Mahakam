#include "mhpch.h"
#include "Renderer.h"

#include "GL.h"

#include "Mahakam/Core/Application.h"

#include <filesystem>
#include <sstream>
#include <fstream>


// TEMPORARY
#include <glad/glad.h>


namespace Mahakam
{
	static Ref<Texture> brdfLut;
	static Ref<Texture> falloffLut;
	static Ref<Texture> spotlightTexture;
	//static Ref<Shader> shadowShader;

	static Ref<Texture> loadOrCreateLUTTexture(const std::string& cachePath, const std::string& shaderPath, TextureFormat format, uint32_t width, uint32_t height)
	{
		if (!std::filesystem::exists(cachePath))
		{
			// Setup LUT shader and framebuffer for capturing
			FrameBufferProps framebufferProps;
			framebufferProps.width = width;
			framebufferProps.height = height;
			framebufferProps.colorAttachments = { format };
			framebufferProps.dontUseDepth = true;
			Ref<FrameBuffer> framebuffer = FrameBuffer::create(framebufferProps);

			Ref<Shader> shader = Shader::create(shaderPath);

			framebuffer->bind();

			shader->bind("LUT");

			GL::clear();

			GL::drawScreenQuad();

			framebuffer->unbind();

			Ref<Texture> lut = framebuffer->getColorTexture(0);

			// Save to cache
			uint32_t size = width * height * 4;
			char* pixels = new char[size];
			lut->readPixels(pixels);
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
			Ref<Texture> lut = Texture2D::create({ width, height, format, TextureFilter::Bilinear, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });
			lut->setData((void*)ss.str().c_str(), 512 * 512 * 4); // TODO: Un-hardcode this value

			return lut;
		}
	}

	Renderer::RendererResults* Renderer::rendererResults = new Renderer::RendererResults;
	Renderer::SceneData* Renderer::sceneData = new Renderer::SceneData;

	std::unordered_map<Ref<Shader>,
		std::unordered_map<Ref<Material>,
		std::unordered_map<Ref<Mesh>,
		std::vector<glm::mat4>>>> Renderer::renderQueue;

	std::vector<Renderer::MeshData> Renderer::transparentQueue;

	void Renderer::init(uint32_t width, uint32_t height)
	{
		MH_PROFILE_FUNCTION();

		GL::init();

		// Initialize
		brdfLut = loadOrCreateLUTTexture("assets/textures/brdf.dat", "assets/shaders/internal/BRDF.yaml", TextureFormat::RG16F, 512, 512);
		falloffLut = loadOrCreateLUTTexture("assets/textures/falloff.dat", "assets/shaders/internal/Falloff.yaml", TextureFormat::R16F, 16, 16);

		spotlightTexture = Texture2D::create("assets/textures/internal/spotlight.png", { TextureFormat::SRGB_DXT1, TextureFilter::Bilinear, TextureWrapMode::ClampBorder, TextureWrapMode::ClampBorder });

		//shadowShader = Shader::create("assets/shaders/internal/Shadow.yaml");

		sceneData->cameraBuffer = UniformBuffer::create(sizeof(CameraData));

		// Create gbuffer
		FrameBufferProps gProps;
		gProps.width = width;
		gProps.height = height;
		gProps.colorAttachments = {
			TextureFormat::RGBA8, // RGB - Albedo, A - Occlussion
			TextureFormat::RGBA8, // RG - Unused, B - Metallic, A - Roughness
			TextureFormat::RG11B10F, // RGB - Emission (not affected by light)
			TextureFormat::RGB10A2 }; // RGB - World normal, A - Unused
			//TextureFormat::RG11B10F }; // RGB - World position offset
		gProps.depthAttachment = TextureFormat::Depth24; // Mutable

		sceneData->gBuffer = FrameBuffer::create(gProps);

		// Create HDR lighting framebuffer
		FrameBufferProps lightingProps;
		lightingProps.width = width;
		lightingProps.height = height;
		lightingProps.colorAttachments = { TextureFormat::RG11B10F };

		sceneData->hdrFrameBuffer = FrameBuffer::create(lightingProps);

		// Create viewport framebuffer
		FrameBufferProps viewportProps;
		viewportProps.width = width;
		viewportProps.height = height;
		viewportProps.colorAttachments = { TextureFormat::RGBA8 };

		sceneData->viewportFramebuffer = FrameBuffer::create(viewportProps);

		// Create lighting shader & material
		Ref<Shader> deferredShader = Shader::create("assets/shaders/internal/DeferredPerLight.yaml");
		sceneData->deferredMaterial = Material::create(deferredShader);
		sceneData->deferredMaterial->setTexture("u_BRDFLUT", 2, brdfLut);
		sceneData->deferredMaterial->setTexture("u_AttenuationLUT", 3, falloffLut);
		sceneData->deferredMaterial->setTexture("u_GBuffer0", 4, sceneData->gBuffer->getColorTexture(0));
		sceneData->deferredMaterial->setTexture("u_GBuffer1", 5, sceneData->gBuffer->getColorTexture(1));
		sceneData->deferredMaterial->setTexture("u_GBuffer2", 6, sceneData->gBuffer->getColorTexture(3));
		sceneData->deferredMaterial->setTexture("u_Depth", 7, sceneData->gBuffer->getDepthTexture());
		sceneData->deferredMaterial->setTexture("u_LightCookie", 8, spotlightTexture);
		//sceneData->deferredMaterial->setTexture("u_GBuffer3", 8, sceneData->gBuffer->getColorTexture(4));

		// Create tonemapping shader & material
		Ref<Shader> tonemappingShader = Shader::create("assets/shaders/internal/Tonemapping.yaml");
		sceneData->tonemappingMaterial = Material::create(tonemappingShader);
		sceneData->tonemappingMaterial->setTexture("u_Albedo", 0, sceneData->hdrFrameBuffer->getColorTexture(0));
	}

	void Renderer::Shutdown()
	{
		GL::Shutdown();

		renderQueue.clear();
		transparentQueue.clear();

		brdfLut = nullptr;
		falloffLut = nullptr;
		spotlightTexture = nullptr;
		//shadowShader = nullptr;

		delete rendererResults;
		delete sceneData;
	}

	void Renderer::onWindowResie(uint32_t width, uint32_t height)
	{
		GL::setViewport(0, 0, width, height);

		// Resize gbuffer and viewport buffer
		sceneData->gBuffer->resize(width, height);
		sceneData->hdrFrameBuffer->resize(width, height);
		sceneData->viewportFramebuffer->resize(width, height);

		// The buffers have been recreated, update them
		sceneData->deferredMaterial->setTexture("u_GBuffer0", 4, sceneData->gBuffer->getColorTexture(0));
		sceneData->deferredMaterial->setTexture("u_GBuffer1", 5, sceneData->gBuffer->getColorTexture(1));
		sceneData->deferredMaterial->setTexture("u_GBuffer2", 6, sceneData->gBuffer->getColorTexture(3));
		sceneData->deferredMaterial->setTexture("u_Depth", 7, sceneData->gBuffer->getDepthTexture());
		//sceneData->deferredMaterial->setTexture("u_GBuffer3", 8, sceneData->gBuffer->getColorTexture(4));

		sceneData->tonemappingMaterial->setTexture("u_Albedo", 0, sceneData->hdrFrameBuffer->getColorTexture(0));
	}

	void Renderer::beginScene(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment)
	{
		MH_PROFILE_FUNCTION();

		sceneData->environment = environment;

		// Setup camera matrices
		CameraData cameraData(cam, transform);

		sceneData->cameraBuffer->setData(&cameraData, 0, sizeof(CameraData));

		// Clear render queues
		renderQueue.clear();
		transparentQueue.clear();
	}

	void Renderer::endScene()
	{
		MH_PROFILE_FUNCTION();

		// Pass 1 - Geometry
		renderGeometryPass();

		// Render wireframe
		if (sceneData->wireframe)
		{
			sceneData->gBuffer->blit(sceneData->viewportFramebuffer);

			rendererResults->triCount /= 3;
			return;
		}

		// Pass 2 - Lighting
		renderLightingPass();

		// Pass 3 - Render back into main viewport (Can't use the lighting buffer directly, since it's in HDR)
		renderFinalPass();

		rendererResults->triCount /= 3;
	}

	void Renderer::enableWireframe(bool enable)
	{
		sceneData->wireframe = enable;
	}

	void Renderer::submit(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material)
	{
		const Ref<Shader>& shader = material->getShader();

		renderQueue[shader][material][mesh].push_back(transform);
	}

	void Renderer::submitTransparent(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material)
	{
		MH_CORE_BREAK("Transparency not yet supported ;_;!")

		//float depth = (sceneData->viewProjectionMatrix * transform[3]).z;

		//transparentQueue.push_back({ depth, mesh, material, transform });
	}

	void Renderer::drawOpaqueQueue()
	{
		MH_PROFILE_FUNCTION();

		for (auto& shaderPair : renderQueue)
		{
			auto& shader = shaderPair.first;
			//shader->bind();

			for (auto& materialPair : shaderPair.second)
			{
				// TODO: Use a UBO instead of rebinding the uniforms each time
				auto& material = materialPair.first;

				if (shader->HasShaderPass("GEOMETRY"))
				{
					material->bindShader("GEOMETRY");
					material->bind();

					// TODO: Use batching at this point?
					for (auto& meshPair : materialPair.second)
					{
						meshPair.first->bind();
						for (auto& transform : meshPair.second)
						{
							material->setTransform(transform);

							rendererResults->drawCalls += 1;
							rendererResults->vertexCount += meshPair.first->getVertexCount();
							rendererResults->triCount += meshPair.first->getIndexCount();

							GL::drawIndexed(meshPair.first->getIndexCount());
						}
					}
				}
			}
		}
	}

	void Renderer::drawTransparentQueue()
	{
		MH_PROFILE_FUNCTION();

		if (transparentQueue.size() > 0)
		{
			// Sort transparent queue
			std::qsort(transparentQueue.data(),
				transparentQueue.size(),
				sizeof(decltype(transparentQueue)::value_type),
				[](const void* x, const void* y) {
				const MeshData arg1 = *static_cast<const MeshData*>(x);
				const MeshData arg2 = *static_cast<const MeshData*>(y);

				const auto cmp = arg1.depth - arg2.depth;

				if (cmp < 0) return -1;
				if (cmp > 0) return 1;
				return 0;
			});

			GL::setBlendMode(RendererAPI::BlendMode::SrcAlpha, RendererAPI::BlendMode::OneMinusSrcAlpha, true);
			for (auto& data : transparentQueue)
			{
				const Ref<Material>& material = data.material;

				material->bindShader("FORWARD");
				material->bind();
				material->setTransform(data.transform);
				data.mesh->bind();

				rendererResults->drawCalls += 1;
				rendererResults->vertexCount += data.mesh->getVertexCount();
				rendererResults->triCount += data.mesh->getIndexCount();

				GL::drawIndexed(data.mesh->getIndexCount());
			}
			GL::setBlendMode(RendererAPI::BlendMode::One, RendererAPI::BlendMode::One, false);
		}
	}

	void Renderer::drawShadowQueue()
	{
		MH_PROFILE_FUNCTION();

		for (auto& shaderPair : renderQueue)
		{
			if (shaderPair.first->HasShaderPass("SHADOW"))
				shaderPair.first->bind("SHADOW");
			//else
			//	shadowShader->bind("SHADOW");

			for (auto& materialPair : shaderPair.second)
			{
				materialPair.first->bind();

				for (auto& meshPair : materialPair.second)
				{
					meshPair.first->bind();
					for (auto& transform : meshPair.second)
					{
						materialPair.first->setTransform(transform);

						rendererResults->drawCalls += 1;
						rendererResults->vertexCount += meshPair.first->getVertexCount();
						rendererResults->triCount += meshPair.first->getIndexCount();

						GL::drawIndexed(meshPair.first->getIndexCount());
					}
				}
			}
		}
	}

	void Renderer::drawSkybox()
	{
		MH_PROFILE_FUNCTION();

		sceneData->environment.skyboxMaterial->bindShader("GEOMETRY");
		sceneData->environment.skyboxMaterial->bind();
		drawScreenQuad();
	}

	void Renderer::drawScreenQuad()
	{
		rendererResults->drawCalls += 1;
		rendererResults->vertexCount += 4;
		rendererResults->triCount += 6;

		GL::drawScreenQuad();
	}

	void Renderer::drawInstancedSphere(uint32_t amount)
	{
		Ref<Mesh> invertedSphere = GL::GetInvertedSphere();

		rendererResults->drawCalls += 1;
		rendererResults->vertexCount += amount * invertedSphere->getVertexCount();
		rendererResults->triCount += amount * invertedSphere->getIndexCount();

		invertedSphere->bind();

		GL::drawInstanced(invertedSphere->getIndexCount(), amount);
	}

	void Renderer::drawInstancedPyramid(uint32_t amount)
	{
		Ref<Mesh> invertedPyramid = GL::GetInvertedPyramid();

		rendererResults->drawCalls += 1;
		rendererResults->vertexCount += amount * invertedPyramid->getVertexCount();
		rendererResults->triCount += amount * invertedPyramid->getIndexCount();

		invertedPyramid->bind();

		GL::drawInstanced(invertedPyramid->getIndexCount(), amount);
	}

	void Renderer::renderGeometryPass()
	{
		MH_PROFILE_RENDERING_FUNCTION();

		GL::setFillMode(!sceneData->wireframe);

		// Setup drawcalls
		rendererResults->drawCalls = 0;
		rendererResults->vertexCount = 0;
		rendererResults->triCount = 0;

		// Bind and clear gBuffer
		sceneData->gBuffer->bind();
		GL::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		GL::clear();

		// Bind matrix buffer
		sceneData->cameraBuffer->bind(0);

		// Render opaque queue
		drawOpaqueQueue();

		// TODO: Move after deferred rendering
		// Use forward rendering, somehow, find a way i guess?
		// Render transparent queue
		drawTransparentQueue();

		sceneData->gBuffer->unbind();

		GL::setFillMode(true);
	}

	void Renderer::renderLightingPass()
	{
		MH_PROFILE_RENDERING_FUNCTION();

		// Blit depth buffer from gBuffer
		sceneData->gBuffer->blit(sceneData->hdrFrameBuffer, false, true);

		// Bind and clear lighting buffer
		sceneData->hdrFrameBuffer->bind();
		GL::setClearColor({ 1.0f, 0.06f, 0.94f, 1.0f });
		GL::clear(true, false);

		sceneData->deferredMaterial->setTexture("u_IrradianceMap", 0, sceneData->environment.irradianceMap);
		sceneData->deferredMaterial->setTexture("u_SpecularMap", 1, sceneData->environment.specularMap);

		// Ambient lighting and don't write or read depth
		GL::enableZWriting(false);
		GL::enableZTesting(false);

		// Directional lights + ambient
		{
			MH_PROFILE_RENDERING_SCOPE("Renderer::renderLightingPass - Directional lights");

			uint32_t amountSize = 16;
			uint32_t amount = (uint32_t)sceneData->environment.directionalLights.size();

			if (amount > 0)
			{
				uint32_t lightSize = sizeof(DirectionalLight);
				uint32_t bufferSize = amountSize + amount * lightSize;

				if (!sceneData->directionalLightBuffer || sceneData->directionalLightBuffer->getSize() != bufferSize)
					sceneData->directionalLightBuffer = StorageBuffer::create(bufferSize);

				sceneData->directionalLightBuffer->setData(&amount, 0, sizeof(int));
				sceneData->directionalLightBuffer->setData(&sceneData->environment.directionalLights[0], amountSize, amount * lightSize);
			}
			else if (!sceneData->directionalLightBuffer || sceneData->directionalLightBuffer->getSize() != amountSize)
			{
				sceneData->directionalLightBuffer = StorageBuffer::create(amountSize);

				sceneData->directionalLightBuffer->setData(&amount, 0, sizeof(int));
			}

			sceneData->deferredMaterial->bindShader("DIRECTIONAL");
			sceneData->deferredMaterial->bind();
			sceneData->directionalLightBuffer->bind(1);

			drawScreenQuad();
		}

		// Render additional lights with additive blend mode
		GL::setBlendMode(RendererAPI::BlendMode::One, RendererAPI::BlendMode::One, true);
		GL::enableZTesting(true);
		glDepthFunc(GL_GEQUAL);

		// Point lights
		{
			MH_PROFILE_RENDERING_SCOPE("Renderer::renderLightingPass - Point lights");

			uint32_t amount = (uint32_t)sceneData->environment.pointLights.size();

			if (amount > 0)
			{
				uint32_t lightSize = sizeof(PointLight);
				uint32_t bufferSize = amount * lightSize;

				if (!sceneData->pointLightBuffer || sceneData->pointLightBuffer->getSize() != bufferSize)
					sceneData->pointLightBuffer = StorageBuffer::create(bufferSize);

				sceneData->pointLightBuffer->setData(&sceneData->environment.pointLights[0], 0, bufferSize);

				sceneData->deferredMaterial->bindShader("POINT");
				sceneData->deferredMaterial->bind();
				sceneData->pointLightBuffer->bind(1);

				drawInstancedSphere(amount);
			}
		}

		// Spot lights
		{
			MH_PROFILE_RENDERING_SCOPE("Renderer::renderLightingPass - Spot lights");

			uint32_t amount = (uint32_t)sceneData->environment.spotLights.size();

			if (amount > 0)
			{
				uint32_t lightSize = sizeof(SpotLight);
				uint32_t bufferSize = amount * lightSize;

				if (!sceneData->spotLightBuffer || sceneData->spotLightBuffer->getSize() != bufferSize)
					sceneData->spotLightBuffer = StorageBuffer::create(bufferSize);

				sceneData->spotLightBuffer->setData(&sceneData->environment.spotLights[0], 0, bufferSize);

				sceneData->deferredMaterial->bindShader("SPOT");
				sceneData->deferredMaterial->bind();
				sceneData->spotLightBuffer->bind(1);

				drawInstancedPyramid(amount);
			}
		}
		glDepthFunc(GL_LEQUAL);

		// Disable blending
		GL::setBlendMode(RendererAPI::BlendMode::One, RendererAPI::BlendMode::One, false);
		GL::enableZTesting(true);

		// Render skybox
		drawSkybox();

		sceneData->hdrFrameBuffer->unbind();
	}

	void Renderer::renderFinalPass()
	{
		MH_PROFILE_RENDERING_FUNCTION();

		sceneData->gBuffer->blit(sceneData->viewportFramebuffer, false, true);

		sceneData->viewportFramebuffer->bind();
		GL::setClearColor({ 1.0f, 0.06f, 0.94f, 1.0f });
		GL::clear(true, false);

		// HDR Tonemapping
		sceneData->tonemappingMaterial->bindShader("POSTPROCESSING");
		sceneData->tonemappingMaterial->bind();

		GL::enableZTesting(false);
		GL::enableZWriting(false);
		drawScreenQuad();
		GL::enableZWriting(true);
		GL::enableZTesting(true);

		sceneData->viewportFramebuffer->unbind();
	}
}