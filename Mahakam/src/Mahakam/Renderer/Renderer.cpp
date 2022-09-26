#include "Mahakam/mhpch.h"
#include "Renderer.h"

#include "Buffer.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include "GL.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "ParticleSystem.h"
#include "Shader.h"
#include "RenderData.h"

#include "Mahakam/Math/Bounds.h"

#include "RenderPasses/GeometryRenderPass.h"
#include "RenderPasses/LightingRenderPass.h"
#include "RenderPasses/ParticleRenderPass.h"
#include "RenderPasses/TonemappingRenderPass.h"

#include <filesystem>
#include <sstream>
#include <fstream>


namespace Mahakam
{
	Renderer::RendererData* Renderer::s_RendererData;
	SceneData* Renderer::s_SceneData;

	void Renderer::Init(uint32_t width, uint32_t height)
	{
		MH_PROFILE_FUNCTION();

		s_RendererData = new RendererData;
		s_SceneData = new SceneData;

		s_RendererData->Width = width;
		s_RendererData->Height = height;

		GL::Init();

		// Initialize camera buffer
		s_SceneData->cameraBuffer = UniformBuffer::Create(sizeof(CameraData));
		s_SceneData->UniformValueBuffer = UniformBuffer::Create(2 << 14); // 16KB

		// Initialize default material
		// TODO: Move to a seperate pass
		Ref<Shader> unlitColorShader = Shader::Create("assets/shaders/internal/UnlitColor.shader"); // TODO: Use the asset system to load it
		s_RendererData->UnlitMaterial = Material::Create(Asset<Shader>(unlitColorShader));
		s_RendererData->UnlitMaterial->SetFloat3("u_Color", { 0.0f, 1.0f, 0.0f });
	}

	void Renderer::Shutdown()
	{
		GL::Shutdown();

		s_RendererData->RenderPasses.clear();
		s_RendererData->FrameBuffers.clear();
		s_RendererData->GBuffer = nullptr;
		s_RendererData->ViewportFramebuffer = nullptr;
		s_RendererData->UnlitMaterial = nullptr;

		delete s_RendererData;
		delete s_SceneData;
	}

	//void Renderer::OnWindowResizeImpl(uint32_t width, uint32_t height)
	MH_DEFINE_FUNC(Renderer::OnWindowResizeImpl, void, uint32_t width, uint32_t height)
	{
		s_RendererData->Width = width;
		s_RendererData->Height = height;

		GL::SetViewport(0, 0, width, height);

		for (auto& renderPass : s_RendererData->RenderPasses)
			renderPass->OnWindowResize(width, height);
	};

	//void Renderer::SetRenderPassesImpl(const std::vector<Ref<RenderPass>>& renderPasses)
	MH_DEFINE_FUNC(Renderer::SetRenderPassesImpl, void, const std::vector<Ref<RenderPass>>& renderPasses)
	{
		s_RendererData->RenderPasses = renderPasses;

		for (auto& renderPass : s_RendererData->RenderPasses)
			renderPass->Init(s_RendererData->Width, s_RendererData->Height);
	};

	//void Renderer::BeginSceneImpl(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment)
	MH_DEFINE_FUNC(Renderer::BeginSceneImpl, void, const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment)
	{
		s_SceneData->environment = environment;

		// Setup camera matrices
		s_SceneData->cameraData = CameraData(cam, glm::vec2(s_RendererData->Width, s_RendererData->Height), transform);

		s_SceneData->cameraBuffer->Bind(0);
		s_SceneData->cameraBuffer->SetData(&s_SceneData->cameraData, 0, sizeof(CameraData));

		// Setup uniforms buffer
		s_SceneData->UniformValueBuffer->Bind(3);

		// Setup results
		s_RendererData->RendererResults.drawCalls = 0;
		s_RendererData->RendererResults.vertexCount = 0;
		s_RendererData->RendererResults.triCount = 0;
	};

	//void Renderer::EndSceneImpl()
	MH_DEFINE_FUNC(Renderer::EndSceneImpl, void)
	{
		// Sort the render queue
		std::sort(s_SceneData->renderQueue.begin(), s_SceneData->renderQueue.end());
		std::sort(s_SceneData->particleQueue.begin(), s_SceneData->particleQueue.end());

		// Render each render pass
		s_RendererData->GBuffer = s_RendererData->RenderPasses[0]->GetFrameBuffer();

		Ref<FrameBuffer> prevBuffer = nullptr;
		for (uint32_t i = 0; i < s_RendererData->RenderPasses.size(); i++)
		{
			if (s_RendererData->RenderPasses[i]->Render(s_SceneData, prevBuffer))
				prevBuffer = s_RendererData->RenderPasses[i]->GetFrameBuffer();
		}

		s_RendererData->ViewportFramebuffer = prevBuffer;

		// Render bounding boxes
		/*if (sceneData->boundingBox)
		{
			rendererData->viewportFramebuffer->Bind();
			GL::SetFillMode(false);

			rendererData->unlitMaterial->BindShader("GEOMETRY");
			rendererData->unlitMaterial->Bind(sceneData->UniformBuffer);

			auto wireMesh = GL::GetCube();
			wireMesh->Bind();

			for (uint64_t drawID : sceneData->renderQueue)
			{
				const uint64_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
				Ref<SubMesh>& mesh = sceneData->meshIDLookup[meshID];

				const uint64_t transformID = drawID & 0xFFFFULL;
				const glm::mat4& transform = sceneData->transformIDLookup[transformID];

				const Bounds transformedBounds = Bounds::TransformBounds(mesh->GetBounds(), transform);

				const glm::vec3 scale = transformedBounds.Max - transformedBounds.Min;
				const glm::vec3 center = transformedBounds.Min + scale / 2.0f;

				const glm::mat4 wireTransform = glm::translate(glm::mat4(1.0f), center)
					* glm::scale(glm::mat4(1.0f), scale);

				rendererData->unlitMaterial->SetTransform(wireTransform);

				Renderer::AddPerformanceResult(wireMesh->GetVertexCount(), wireMesh->GetIndexCount());

				GL::DrawIndexed(wireMesh->GetIndexCount());
			}

			GL::SetFillMode(true);
			rendererData->viewportFramebuffer->Unbind();
		}*/

		// Normalize results
		s_RendererData->RendererResults.triCount /= 3;

		// Clear render queues
		s_SceneData->renderQueue.clear();

		s_SceneData->shaderRefLookup.clear();
		s_SceneData->materialRefLookup.clear();
		s_SceneData->meshRefLookup.clear();

		s_SceneData->shaderIDLookup.clear();
		s_SceneData->materialIDLookup.clear();
		s_SceneData->meshIDLookup.clear();
		s_SceneData->transformIDLookup.clear();
	};

	//void Renderer::SubmitImpl(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material)
	MH_DEFINE_FUNC(Renderer::SubmitImpl, void, const glm::mat4& transform, Ref<SubMesh> mesh, Ref<Material> material)
	{
		// Add shader if it doesn't exist
		uint64_t shaderID;
		Ref<Shader> shader = material->GetShader().RefPtr();
		auto shaderIter = s_SceneData->shaderRefLookup.find(shader);
		if (shaderIter == s_SceneData->shaderRefLookup.end())
		{
			shaderID = s_SceneData->shaderRefLookup.size();
			s_SceneData->shaderRefLookup[shader] = shaderID;
			s_SceneData->shaderIDLookup[shaderID] = shader;
		}
		else
		{
			shaderID = shaderIter->second;
		}

		// Add material if it doesn't exist
		uint64_t materialID;
		auto matIter = s_SceneData->materialRefLookup.find(material);
		if (matIter == s_SceneData->materialRefLookup.end())
		{
			materialID = s_SceneData->materialRefLookup.size();
			s_SceneData->materialRefLookup[material] = materialID;
			s_SceneData->materialIDLookup[materialID] = material;
		}
		else
		{
			materialID = matIter->second;
		}

		// Add mesh if it doesn't exist
		uint64_t meshID;
		auto meshIter = s_SceneData->meshRefLookup.find(mesh);
		if (meshIter == s_SceneData->meshRefLookup.end())
		{
			meshID = s_SceneData->meshRefLookup.size();
			s_SceneData->meshRefLookup[mesh] = meshID;
			s_SceneData->meshIDLookup[meshID] = mesh;
		}
		else
		{
			meshID = meshIter->second;
		}

		// Add transform
		uint64_t transformID = s_SceneData->transformIDLookup.size();
		s_SceneData->transformIDLookup[transformID] = transform;

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

			float depth = (s_SceneData->cameraData.u_m4_VP * transform[3]).z;

			uint64_t depthInt = (uint64_t)(depth * (1ULL << 31ULL));

			drawID |= ((depthInt & 0xFFFFFFFFULL) << 30ULL);
		}

		s_SceneData->renderQueue.push_back(drawID);
	};

	//void Renderer::SubmitParticlesImpl(const glm::mat4& transform, const ParticleSystem& particles)
	MH_DEFINE_FUNC(Renderer::SubmitParticlesImpl, void, const glm::mat4& transform, const ParticleSystem& particles)
	{
		uint64_t particleID = s_SceneData->particleIDLookup.size();
		s_SceneData->particleIDLookup[particleID] = particles;

		uint64_t transformID = s_SceneData->transformIDLookup.size();
		s_SceneData->transformIDLookup[transformID] = transform;

		float depth = (s_SceneData->cameraData.u_m4_VP * transform[3]).z;

		uint64_t depthInt = (uint64_t)(depth * (1ULL << 31ULL));

		uint64_t drawID = ((depthInt & 0xFFFFFFFFULL) << 32ULL);
		drawID |= ((particleID & 0xFFFFULL) << 16ULL);
		drawID |= (transformID & 0xFFFFULL);

		s_SceneData->particleQueue.push_back(drawID);
	};

	//void Renderer::DrawSkyboxImpl(SceneData* sceneData)
	MH_DEFINE_FUNC(Renderer::DrawSkyboxImpl, void, SceneData* sceneData)
	{
		if (sceneData->environment.SkyboxMaterial)
		{
			sceneData->environment.SkyboxMaterial->BindShader("GEOMETRY");
			sceneData->environment.SkyboxMaterial->Bind(sceneData->UniformValueBuffer);
			DrawScreenQuad();
		}
	};

	//void Renderer::DrawScreenQuadImpl()
	MH_DEFINE_FUNC(Renderer::DrawScreenQuadImpl, void)
	{
		AddPerformanceResult(4, 6);

		GL::DrawScreenQuad();
	};

	//void Renderer::DrawInstancedSphereImpl(uint32_t amount)
	MH_DEFINE_FUNC(Renderer::DrawInstancedSphereImpl, void, uint32_t amount)
	{
		Ref<SubMesh> invertedSphere = GL::GetInvertedSphere();

		AddPerformanceResult(amount * invertedSphere->GetVertexCount(), amount * invertedSphere->GetIndexCount());

		invertedSphere->Bind();

		GL::DrawInstanced(invertedSphere->GetIndexCount(), amount);
	};

	//void Renderer::DrawInstancedPyramidImpl(uint32_t amount)
	MH_DEFINE_FUNC(Renderer::DrawInstancedPyramidImpl, void, uint32_t amount)
	{
		Ref<SubMesh> invertedPyramid = GL::GetInvertedPyramid();

		AddPerformanceResult(amount * invertedPyramid->GetVertexCount(), amount * invertedPyramid->GetIndexCount());

		invertedPyramid->Bind();

		GL::DrawInstanced(invertedPyramid->GetIndexCount(), amount);
	};

	//void Renderer::EnableWireframeImpl(bool enable)
	MH_DEFINE_FUNC(Renderer::EnableWireframeImpl, void, bool enable)
	{
		s_SceneData->wireframe = enable;
	};

	//void Renderer::EnableBoundingBoxImpl(bool enable)
	MH_DEFINE_FUNC(Renderer::EnableBoundingBoxImpl, void, bool enable)
	{
		s_SceneData->boundingBox = enable;
	};

	//void Renderer::EnableGBufferImpl(bool enable)
	MH_DEFINE_FUNC(Renderer::EnableGBufferImpl, void, bool enable)
	{
		s_SceneData->gBuffer = enable;
	};

	//bool Renderer::HasWireframeEnabledImpl()
	MH_DEFINE_FUNC(Renderer::HasWireframeEnabledImpl, bool)
	{
		return s_SceneData->wireframe;
	};

	//bool Renderer::HasBoundingBoxEnabledImpl()
	MH_DEFINE_FUNC(Renderer::HasBoundingBoxEnabledImpl, bool)
	{
		return s_SceneData->boundingBox;
	};

	//bool Renderer::HasGBufferEnabledImpl()
	MH_DEFINE_FUNC(Renderer::HasGBufferEnabledImpl, bool)
	{
		return s_SceneData->gBuffer;
	};

	//void Renderer::AddFrameBufferImpl(const std::string& name, WeakRef<FrameBuffer> frameBuffer)
	MH_DEFINE_FUNC(Renderer::AddFrameBufferImpl, void, const std::string& name, WeakRef<FrameBuffer> frameBuffer)
	{
		s_RendererData->FrameBuffers[name] = frameBuffer;
	};

	//const robin_hood::unordered_map<std::string, WeakRef<FrameBuffer>>& Renderer::GetFrameBuffersImpl()
	MH_DEFINE_FUNC(Renderer::GetFrameBuffersImpl, const Renderer::FrameBufferMap&)
	{
		auto iter = s_RendererData->FrameBuffers.begin();
		while (iter != s_RendererData->FrameBuffers.end())
		{
			if (iter->second.expired())
				iter = s_RendererData->FrameBuffers.erase(iter);
			else
				iter++;
		}

		return s_RendererData->FrameBuffers;
	};

	//Ref<FrameBuffer> Renderer::GetGBufferImpl()
	MH_DEFINE_FUNC(Renderer::GetGBufferImpl, Ref<FrameBuffer>)
	{
		return s_RendererData->GBuffer;
	};

	//Ref<FrameBuffer> Renderer::GetFrameBufferImpl()
	MH_DEFINE_FUNC(Renderer::GetFrameBufferImpl, Ref<FrameBuffer>)
	{
		return s_RendererData->ViewportFramebuffer;
	};

	//void Renderer::AddPerformanceResultImpl(uint32_t vertexCount, uint32_t indexCount)
	MH_DEFINE_FUNC(Renderer::AddPerformanceResultImpl, void, uint32_t vertexCount, uint32_t indexCount)
	{
		s_RendererData->RendererResults.drawCalls++;
		s_RendererData->RendererResults.vertexCount += vertexCount;
		s_RendererData->RendererResults.triCount += indexCount;
	};

	//const RendererResults& Renderer::GetPerformanceResultsImpl()
	MH_DEFINE_FUNC(Renderer::GetPerformanceResultsImpl, const RendererResults&)
	{
		return s_RendererData->RendererResults;
	};
}