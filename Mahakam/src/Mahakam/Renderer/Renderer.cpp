#include "mhpch.h"
#include "Renderer.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "GL.h"

#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "ParticleSystem.h"

#include "RenderPasses.h"

#include <filesystem>
#include <sstream>
#include <fstream>


namespace Mahakam
{
	Renderer* Renderer::s_Instance = nullptr;

	Renderer::Renderer(uint32_t width, uint32_t height)
	{
		MH_PROFILE_FUNCTION();

		rendererData.width = width;
		rendererData.height = height;

		GL::Init();

		// Initialize camera buffer
		sceneData.cameraBuffer = UniformBuffer::Create(sizeof(CameraData));

		// Initialize default material
		Ref<Shader> unlitColorShader = Shader::Create("assets/shaders/internal/UnlitColor.yaml");
		rendererData.unlitMaterial = Material::Create(unlitColorShader);
		rendererData.unlitMaterial->SetFloat3("u_Color", { 0.0f, 1.0f, 0.0f });
	}

	Renderer::~Renderer()
	{
		GL::Shutdown();

		rendererData.viewportFramebuffer = nullptr;
		rendererData.unlitMaterial = nullptr;
	}

	Renderer* Renderer::GetInstance()
	{
		MH_OVERRIDE_FUNC(RendererGetInstance);

		return s_Instance;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		auto instance = GetInstance();

		instance->rendererData.width = width;
		instance->rendererData.height = height;

		GL::SetViewport(0, 0, width, height);

		for (auto& renderPass : instance->rendererData.renderPasses)
			renderPass->OnWindowResize(width, height);
	}

	void Renderer::SetRenderPassesImpl(const std::vector<Ref<RenderPass>>& renderPasses)
	{
		rendererData.renderPasses = renderPasses;

		for (auto& renderPass : rendererData.renderPasses)
			renderPass->Init(rendererData.width, rendererData.height);
	}

	void Renderer::BeginSceneImpl(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment)
	{
		MH_PROFILE_FUNCTION();

		sceneData.environment = environment;

		// Setup camera matrices
		sceneData.cameraData = CameraData(cam, glm::vec2(rendererData.width, rendererData.height), transform);

		sceneData.cameraBuffer->Bind(0);
		sceneData.cameraBuffer->SetData(&sceneData.cameraData, 0, sizeof(CameraData));

		// Setup results
		rendererData.rendererResults.drawCalls = 0;
		rendererData.rendererResults.vertexCount = 0;
		rendererData.rendererResults.triCount = 0;
	}

	void Renderer::EndSceneImpl()
	{
		MH_PROFILE_FUNCTION();

		// Sort the render queue
		std::sort(sceneData.renderQueue.begin(), sceneData.renderQueue.end());
		std::sort(sceneData.particleQueue.begin(), sceneData.particleQueue.end());

		// Render each render pass
		rendererData.gBuffer = rendererData.renderPasses[0]->GetFrameBuffer();

		Ref<FrameBuffer> prevBuffer = nullptr;
		for (uint32_t i = 0; i < rendererData.renderPasses.size(); i++)
		{
			if (rendererData.renderPasses[i]->Render(&sceneData, prevBuffer))
				prevBuffer = rendererData.renderPasses[i]->GetFrameBuffer();
		}

		rendererData.viewportFramebuffer = prevBuffer;

		// Render bounding boxes
		if (sceneData.boundingBox)
		{
			rendererData.viewportFramebuffer->Bind();
			GL::SetFillMode(false);

			rendererData.unlitMaterial->BindShader("GEOMETRY");
			rendererData.unlitMaterial->Bind();

			auto wireMesh = GL::GetCube();
			wireMesh->Bind();

			for (uint64_t drawID : sceneData.renderQueue)
			{
				const uint64_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
				Ref<Mesh>& mesh = sceneData.meshIDLookup[meshID];

				const uint64_t transformID = drawID & 0xFFFFULL;
				const glm::mat4& transform = sceneData.transformIDLookup[transformID];

				const Mesh::Bounds transformedBounds = Mesh::TransformBounds(mesh->GetBounds(), transform);

				const glm::vec3 scale = transformedBounds.max - transformedBounds.min;
				const glm::vec3 center = transformedBounds.min + scale / 2.0f;

				const glm::mat4 wireTransform = glm::translate(glm::mat4(1.0f), center)
					* glm::scale(glm::mat4(1.0f), scale);

				rendererData.unlitMaterial->SetTransform(wireTransform);

				Renderer::AddPerformanceResult(wireMesh->GetVertexCount(), wireMesh->GetIndexCount());

				GL::DrawIndexed(wireMesh->GetIndexCount());
			}

			GL::SetFillMode(true);
			rendererData.viewportFramebuffer->Unbind();
		}

		// Normalize results
		rendererData.rendererResults.triCount /= 3;

		// Clear render queues
		sceneData.renderQueue.clear();

		sceneData.shaderRefLookup.clear();
		sceneData.materialRefLookup.clear();
		sceneData.meshRefLookup.clear();

		sceneData.shaderIDLookup.clear();
		sceneData.materialIDLookup.clear();
		sceneData.meshIDLookup.clear();
		sceneData.transformIDLookup.clear();
	}

	void Renderer::SubmitImpl(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material)
	{
		// Add shader if it doesn't exist
		uint64_t shaderID;
		Ref<Shader> shader = material->GetShader();
		auto shaderIter = sceneData.shaderRefLookup.find(shader);
		if (shaderIter == sceneData.shaderRefLookup.end())
		{
			shaderID = sceneData.shaderRefLookup.size();
			sceneData.shaderRefLookup[shader] = shaderID;
			sceneData.shaderIDLookup[shaderID] = shader;
		}
		else
		{
			shaderID = shaderIter->second;
		}

		// Add material if it doesn't exist
		uint64_t materialID;
		auto matIter = sceneData.materialRefLookup.find(material);
		if (matIter == sceneData.materialRefLookup.end())
		{
			materialID = sceneData.materialRefLookup.size();
			sceneData.materialRefLookup[material] = materialID;
			sceneData.materialIDLookup[materialID] = material;
		}
		else
		{
			materialID = matIter->second;
		}

		// Add mesh if it doesn't exist
		uint64_t meshID;
		auto meshIter = sceneData.meshRefLookup.find(mesh);
		if (meshIter == sceneData.meshRefLookup.end())
		{
			meshID = sceneData.meshRefLookup.size();
			sceneData.meshRefLookup[mesh] = meshID;
			sceneData.meshIDLookup[meshID] = mesh;
		}
		else
		{
			meshID = meshIter->second;
		}

		// Add transform
		uint64_t transformID = sceneData.transformIDLookup.size();
		sceneData.transformIDLookup[transformID] = transform;

		uint64_t drawID = 0;

		if (false) // Fullscreen quad
		{
			drawID |= (3ULL << 62ULL);
		}
		else if (true) // Opaque or AlphaTest
		{
			if (false) // AlphaTest
				drawID |= (1ULL << 62ULL);

			drawID |= ((shaderID & 0x7FFFULL) << 47ULL);
			drawID |= ((materialID & 0x7FFFULL) << 32ULL);
			drawID |= ((meshID & 0xFFFFULL) << 16ULL);
			drawID |= (transformID & 0xFFFFULL);
		}
		else // Transparent
		{
			drawID |= (2ULL << 62ULL);

			float depth = (sceneData.cameraData.u_m4_VP * transform[3]).z;

			uint64_t depthInt = (uint64_t)(depth * ((1ULL << 31ULL) - 1ULL));

			drawID |= ((depthInt & 0xFFFFFFFFULL) << 30ULL);
		}

		sceneData.renderQueue.push_back(drawID);
	}

	void Renderer::SubmitParticlesImpl(const glm::mat4& transform, const ParticleSystem& particles)
	{
		uint64_t particleID = sceneData.particleIDLookup.size();
		sceneData.particleIDLookup[particleID] = particles;

		uint64_t transformID = sceneData.transformIDLookup.size();
		sceneData.transformIDLookup[transformID] = transform;

		float depth = (sceneData.cameraData.u_m4_VP * transform[3]).z;

		uint64_t depthInt = (uint64_t)(depth * ((1ULL << 31ULL) - 1ULL));

		uint64_t drawID = ((depthInt & 0xFFFFFFFFULL) << 32ULL);
		drawID |= ((particleID & 0xFFFFULL) << 16ULL);
		drawID |= (transformID & 0xFFFFULL);

		sceneData.particleQueue.push_back(drawID);
	}

	void Renderer::DrawSkyboxImpl()
	{
		MH_PROFILE_FUNCTION();

		sceneData.environment.skyboxMaterial->BindShader("GEOMETRY");
		sceneData.environment.skyboxMaterial->Bind();
		DrawScreenQuad();
	}

	void Renderer::DrawScreenQuadImpl()
	{
		AddPerformanceResult(4, 6);

		GL::DrawScreenQuad();
	}

	void Renderer::DrawInstancedSphereImpl(uint32_t amount)
	{
		Ref<Mesh> invertedSphere = GL::GetInvertedSphere();

		AddPerformanceResult(amount * invertedSphere->GetVertexCount(), amount * invertedSphere->GetIndexCount());

		invertedSphere->Bind();

		GL::DrawInstanced(invertedSphere->GetIndexCount(), amount);
	}

	void Renderer::DrawInstancedPyramidImpl(uint32_t amount)
	{
		Ref<Mesh> invertedPyramid = GL::GetInvertedPyramid();

		AddPerformanceResult(amount * invertedPyramid->GetVertexCount(), amount * invertedPyramid->GetIndexCount());

		invertedPyramid->Bind();

		GL::DrawInstanced(invertedPyramid->GetIndexCount(), amount);
	}


	void Renderer::AddFrameBufferImpl(const std::string& name, WeakRef<FrameBuffer> frameBuffer)
	{
		rendererData.frameBuffers[name] = frameBuffer;
	}

	const robin_hood::unordered_map<std::string, WeakRef<FrameBuffer>>& Renderer::GetFrameBuffersImpl()
	{
		auto iter = rendererData.frameBuffers.begin();
		while (iter != rendererData.frameBuffers.end())
		{
			if (iter->second.expired())
				iter = rendererData.frameBuffers.erase(iter);
			else
				iter++;
		}

		return rendererData.frameBuffers;
	}
}