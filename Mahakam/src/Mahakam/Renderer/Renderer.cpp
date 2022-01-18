#include "mhpch.h"
#include "Renderer.h"

#include "Mahakam/Core/Application.h"

#include <filesystem>
#include <sstream>
#include <fstream>

namespace Mahakam
{
	static Ref<Texture> brdfLut;

	static Ref<Texture> loadOrCreateBRDF(const std::string& cachePath, uint32_t width, uint32_t height)
	{
		if (!std::filesystem::exists(cachePath))
		{
			// Setup BRDF LUT for lighting
			FrameBufferProps brdfProps;
			brdfProps.width = width;
			brdfProps.height = height;
			brdfProps.colorAttachments = { TextureFormat::RG16F };
			brdfProps.dontUseDepth = true;
			Ref<FrameBuffer> brdfFramebuffer = FrameBuffer::create(brdfProps);

			Ref<Shader> brdfShader = Shader::create("assets/shaders/internal/BRDF.glsl");

			brdfFramebuffer->bind();

			brdfShader->bind();

			GL::clear();

			GL::drawScreenQuad();

			brdfFramebuffer->unbind();

			Ref<Texture> brdfLut = brdfFramebuffer->getColorTexture(0);

			// Save to cache
			uint32_t size = width * height * 4;
			char* pixels = new char[size];
			brdfLut->readPixels(pixels);
			std::ofstream stream(cachePath, std::ios::binary);
			stream.write(pixels, size);

			delete[] pixels;

			return brdfLut;
		}
		else
		{
			// Load from cache
			std::ifstream inStream(cachePath, std::ios::binary);
			std::stringstream ss;
			ss << inStream.rdbuf();
			Ref<Texture> brdfLut = Texture2D::create({ width, height, TextureFormat::RG16F, TextureFilter::Bilinear, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });
			brdfLut->setData((void*)ss.str().c_str(), ss.str().size());

			return brdfLut;
		}
	}

	Renderer::RendererResults* Renderer::rendererResults = new Renderer::RendererResults;
	Renderer::SceneData* Renderer::sceneData = new Renderer::SceneData;

	std::unordered_map<Ref<Shader>,
		std::unordered_map<Ref<Material>,
		std::unordered_map<Ref<Mesh>,
		std::vector<glm::mat4>>>> Renderer::renderQueue;

	std::vector<Renderer::MeshData> Renderer::transparentQueue;

	void Renderer::onWindowResie(uint32_t width, uint32_t height)
	{
		GL::setViewport(0, 0, width, height);

		// Resize gbuffer and viewport buffer
		sceneData->gBuffer->resize(width, height);
		sceneData->viewportFramebuffer->resize(width, height);

		// The buffers have been recreated, update them
		sceneData->lightingMaterial->setTexture("u_Albedo", 3, sceneData->gBuffer->getColorTexture(0));
		sceneData->lightingMaterial->setTexture("u_Specular", 4, sceneData->gBuffer->getColorTexture(1));
		sceneData->lightingMaterial->setTexture("u_Pos", 5, sceneData->gBuffer->getColorTexture(2));
		sceneData->lightingMaterial->setTexture("u_Normal", 6, sceneData->gBuffer->getColorTexture(3));
	}

	void Renderer::init(uint32_t width, uint32_t height)
	{
		MH_PROFILE_FUNCTION();

		GL::init();

		// Initialize
		brdfLut = loadOrCreateBRDF("assets/textures/brdf.dat", 512, 512);

		sceneData->matrixBuffer = UniformBuffer::create(sizeof(glm::vec3) + sizeof(glm::mat4) * 2);

		// Create gbuffer
		FrameBufferProps gProps;
		gProps.width = width;
		gProps.height = height;
		gProps.colorAttachments = { TextureFormat::RGBA8, TextureFormat::RGBA8, TextureFormat::RGBA16F, TextureFormat::RGBA16F };

		sceneData->gBuffer = FrameBuffer::create(gProps);

		// Create viewport framebuffer
		FrameBufferProps viewportProps;
		viewportProps.width = width;
		viewportProps.height = height;
		viewportProps.colorAttachments = { TextureFormat::RGBA8 };

		sceneData->viewportFramebuffer = FrameBuffer::create(viewportProps);

		// Create lighting shader & material
		Ref<Shader> deferredShader = Shader::create("assets/shaders/internal/Deferred.glsl");
		sceneData->lightingMaterial = Material::create(deferredShader);
		sceneData->lightingMaterial->setTexture("u_BRDFLUT", 2, brdfLut);
		sceneData->lightingMaterial->setTexture("u_Albedo", 3, sceneData->gBuffer->getColorTexture(0));
		sceneData->lightingMaterial->setTexture("u_Specular", 4, sceneData->gBuffer->getColorTexture(1));
		sceneData->lightingMaterial->setTexture("u_Pos", 5, sceneData->gBuffer->getColorTexture(2));
		sceneData->lightingMaterial->setTexture("u_Normal", 6, sceneData->gBuffer->getColorTexture(3));
	}

	void Renderer::beginScene(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment)
	{
		MH_PROFILE_FUNCTION();

		sceneData->environment = environment;

		glm::mat4 viewMatrix = glm::inverse(transform);

		// Setup camera matrices
		sceneData->matrixBuffer->setData(&viewMatrix, 0, sizeof(glm::mat4));
		sceneData->matrixBuffer->setData(&transform[3], sizeof(glm::mat4) * 2, sizeof(glm::vec3));
		sceneData->matrixBuffer->setData(&cam.getProjectionMatrix(), sizeof(glm::mat4), sizeof(glm::mat4));

		sceneData->viewProjectionMatrix = cam.getProjectionMatrix() * viewMatrix;

		// Clear render queues
		renderQueue.clear();
		transparentQueue.clear();
	}

	void Renderer::endScene()
	{
		MH_PROFILE_FUNCTION();

		sceneData->gBuffer->bind();

		GL::setClearColor({ 0.1f, 0.1f, 0.1f, 0.1f });
		GL::clear();

		sceneData->lightingMaterial->setTexture("u_IrradianceMap", 0, sceneData->environment.irradianceMap);
		sceneData->lightingMaterial->setTexture("u_SpecularMap", 1, sceneData->environment.specularMap);

		Ref<UniformBuffer> lightBuffer = UniformBuffer::create(2 * 16);

		glm::vec3 lightPos = sceneData->environment.lights[0]->getPosition();
		glm::vec3 lightCol = sceneData->environment.lights[0]->getColor();
		lightBuffer->setData(&lightPos, 0, sizeof(glm::vec3));
		lightBuffer->setData(&lightCol, 16, sizeof(glm::vec3));

		rendererResults->drawCalls = 0;
		rendererResults->vertexCount = 0;
		rendererResults->triCount = 0;

		// Bind uniform buffers
		sceneData->matrixBuffer->bind(0);
		lightBuffer->bind(1);

		// Render opaque queue
		for (auto& shaderPair : renderQueue)
		{
			shaderPair.first->bind();
			//shaderPair.first->bindBuffer("Matrices", 0);
			//shaderPair.first->bindBuffer("Lights", 1);

			for (auto& materialPair : shaderPair.second)
			{
				// TODO: Use a UBO instead of rebinding the uniforms each time
				materialPair.first->bind();

				// TODO: Use batching at this point?
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


		// TODO: Move after deferred rendering
		// Render transparent queue
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

			GL::setBlendMode(true);
			for (auto& data : transparentQueue)
			{
				const Ref<Material>& material = data.material;
				const Ref<Shader>& shader = material->getShader();

				shader->bind();
				//shader->bindBuffer("Matrices", 0);
				//shader->bindBuffer("Lights", 1);
				material->bind();
				material->setTransform(data.transform);
				data.mesh->bind();

				rendererResults->drawCalls += 1;
				rendererResults->vertexCount += data.mesh->getVertexCount();
				rendererResults->triCount += data.mesh->getIndexCount();

				GL::drawIndexed(data.mesh->getIndexCount());
			}
			GL::setBlendMode(false);
		}

		rendererResults->triCount /= 3;

		sceneData->gBuffer->unbind();


		// Do lighting calculations
		sceneData->viewportFramebuffer->bind();

		GL::setClearColor({ 1.0f, 0.1f, 0.1f, 0.1f });
		GL::clear();

		sceneData->matrixBuffer->bind(0);
		lightBuffer->bind(1);

		sceneData->lightingMaterial->getShader()->bind();
		sceneData->lightingMaterial->bind();

		GL::drawScreenQuad();

		sceneData->viewportFramebuffer->unbind();


		// Blit depth from gBuffer to viewport
		sceneData->gBuffer->blitDepth(sceneData->viewportFramebuffer);
	}

	void Renderer::submit(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		MH_PROFILE_FUNCTION();

		const Ref<Shader>& shader = material->getShader();

		renderQueue[shader][material][mesh].push_back(transform);
	}

	void Renderer::submitTransparent(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		MH_PROFILE_FUNCTION();

		float depth = (sceneData->viewProjectionMatrix * transform[3]).z;

		transparentQueue.push_back({ depth, mesh, material, transform });
	}
}