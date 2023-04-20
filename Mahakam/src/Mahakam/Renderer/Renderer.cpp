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
	Scope<Renderer::RendererData> Renderer::s_RendererData;
	Scope<SceneData> Renderer::s_SceneData;

	void Renderer::Init(uint32_t width, uint32_t height)
	{
		MH_PROFILE_FUNCTION();

		s_RendererData = CreateScope<RendererData>();
		s_SceneData = CreateScope<SceneData>();

		s_RendererData->Width = width;
		s_RendererData->Height = height;

		GL::Init();

		// Initialize camera buffer
		s_SceneData->CameraBuffer = UniformBuffer::Create(sizeof(CameraData));
		s_SceneData->UniformValueBuffer = UniformBuffer::Create(2 << 13); // 16KB
	}

	void Renderer::Shutdown()
	{
		GL::Shutdown();

		s_RendererData->RenderPasses.clear();
		s_RendererData->FrameBuffers.clear();
		s_RendererData->GBuffer = nullptr;
		s_RendererData->ViewportFramebuffer = nullptr;

		s_RendererData = nullptr;
		s_SceneData = nullptr;
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

	//void Renderer::BeginSceneImpl(const Camera& cam, const glm::mat4& transform, const EnvironmentData& Environment)
	MH_DEFINE_FUNC(Renderer::BeginSceneImpl, void, const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment)
	{
		s_SceneData->Environment = environment;

		// Setup camera matrices
		s_SceneData->CameraMatrix = CameraData(cam, glm::vec2(s_RendererData->Width, s_RendererData->Height), transform);

		s_SceneData->CameraBuffer->Bind(0);
		s_SceneData->CameraBuffer->SetData(&s_SceneData->CameraMatrix, 0, sizeof(CameraData));

		// Setup uniforms buffer
		s_SceneData->UniformValueBuffer->Bind(3);

		// Setup results
		s_RendererData->FrameResults.DrawCalls = 0;
		s_RendererData->FrameResults.VertexCount = 0;
		s_RendererData->FrameResults.TriCount = 0;
	};

	//void Renderer::EndSceneImpl()
	MH_DEFINE_FUNC(Renderer::EndSceneImpl, void)
	{
		// Sort the render queue
		std::sort(s_SceneData->RenderQueue.begin(), s_SceneData->RenderQueue.end());
		std::sort(s_SceneData->ParticleQueue.begin(), s_SceneData->ParticleQueue.end());

		// Render each render pass
		s_RendererData->GBuffer = s_RendererData->RenderPasses[0]->GetFrameBuffer();

		Asset<FrameBuffer> prevBuffer = nullptr;
		for (auto& renderPass : s_RendererData->RenderPasses)
		{
			if (renderPass->Render(s_SceneData.get(), prevBuffer))
				prevBuffer = renderPass->GetFrameBuffer();
		}

		s_RendererData->ViewportFramebuffer = prevBuffer;

		// Normalize results
		s_RendererData->FrameResults.TriCount /= 3;

		// Clear render queues
		s_SceneData->RenderQueue.clear();

		s_SceneData->ShaderRefLookup.clear();
		s_SceneData->MaterialRefLookup.clear();
		s_SceneData->MeshRefLookup.clear();

		s_SceneData->ShaderIDLookup.clear();
		s_SceneData->MaterialIDLookup.clear();
		s_SceneData->MeshIDLookup.clear();
		s_SceneData->TransformIDLookup.clear();
	};

	//void Renderer::SubmitImpl(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material)
	MH_DEFINE_FUNC(Renderer::SubmitImpl, void, const glm::mat4& transform, Ref<SubMesh> mesh, Asset<Material> material)
	{
		if (!material) return;
		Asset<Shader> shader = material->GetShader();
		if (!shader) return;

		// Add shader if it doesn't exist
		uint64_t shaderID;
		auto shaderIter = s_SceneData->ShaderRefLookup.find(shader);
		if (shaderIter == s_SceneData->ShaderRefLookup.end())
		{
			shaderID = s_SceneData->ShaderRefLookup.size();
			s_SceneData->ShaderRefLookup.insert({ shader, shaderID });
			s_SceneData->ShaderIDLookup.insert({ shaderID, std::move(shader) });
		}
		else
		{
			shaderID = shaderIter->second;
		}

		// Add material if it doesn't exist
		uint64_t materialID;
		auto matIter = s_SceneData->MaterialRefLookup.find(material);
		if (matIter == s_SceneData->MaterialRefLookup.end())
		{
			materialID = s_SceneData->MaterialRefLookup.size();
			s_SceneData->MaterialRefLookup.insert({ material, materialID });
			s_SceneData->MaterialIDLookup.insert({ materialID, std::move(material) });
		}
		else
		{
			materialID = matIter->second;
		}

		// Add mesh if it doesn't exist
		uint64_t meshID;
		auto meshIter = s_SceneData->MeshRefLookup.find(mesh);
		if (meshIter == s_SceneData->MeshRefLookup.end())
		{
			meshID = s_SceneData->MeshRefLookup.size();
			s_SceneData->MeshRefLookup.insert({ mesh, meshID });
			s_SceneData->MeshIDLookup.insert({ meshID, std::move(mesh) });
		}
		else
		{
			meshID = meshIter->second;
		}

		// Add transform
		uint64_t transformID = s_SceneData->TransformIDLookup.size();
		s_SceneData->TransformIDLookup.insert({ transformID, transform });

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

			float depth = (s_SceneData->CameraMatrix.u_m4_VP * transform[3]).z;

			uint64_t depthInt = (uint64_t)(depth * (1ULL << 31ULL));

			drawID |= ((depthInt & 0xFFFFFFFFULL) << 30ULL);
		}

		s_SceneData->RenderQueue.push_back(drawID);
	};

	//void Renderer::SubmitParticlesImpl(const glm::mat4& transform, const ParticleSystem& particles)
	MH_DEFINE_FUNC(Renderer::SubmitParticlesImpl, void, const glm::mat4& transform, const ParticleSystem& particles)
	{
		uint64_t particleID = s_SceneData->ParticleIDLookup.size();
		s_SceneData->ParticleIDLookup[particleID] = particles;

		uint64_t transformID = s_SceneData->TransformIDLookup.size();
		s_SceneData->TransformIDLookup[transformID] = transform;

		float depth = (s_SceneData->CameraMatrix.u_m4_VP * transform[3]).z;

		uint64_t depthInt = (uint64_t)(depth * (1ULL << 31ULL));

		uint64_t drawID = ((depthInt & 0xFFFFFFFFULL) << 32ULL);
		drawID |= ((particleID & 0xFFFFULL) << 16ULL);
		drawID |= (transformID & 0xFFFFULL);

		s_SceneData->ParticleQueue.push_back(drawID);
	};

	//void Renderer::DrawSkyboxImpl(SceneData* sceneData)
	MH_DEFINE_FUNC(Renderer::DrawSkyboxImpl, void, SceneData* sceneData)
	{
		if (sceneData->Environment.SkyboxMaterial)
		{
			sceneData->Environment.SkyboxMaterial->BindShader("GEOMETRY");
			sceneData->Environment.SkyboxMaterial->Bind(*sceneData->UniformValueBuffer);
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
		s_SceneData->Wireframe = enable;
	};

	//void Renderer::EnableBoundingBoxImpl(bool enable)
	MH_DEFINE_FUNC(Renderer::EnableBoundingBoxImpl, void, bool enable)
	{
		s_SceneData->BoundingBox = enable;
	};

	//void Renderer::EnableGBufferImpl(bool enable)
	MH_DEFINE_FUNC(Renderer::EnableGBufferImpl, void, bool enable)
	{
		s_SceneData->GBuffer = enable;
	};

	//bool Renderer::HasWireframeEnabledImpl()
	MH_DEFINE_FUNC(Renderer::HasWireframeEnabledImpl, bool)
	{
		return s_SceneData->Wireframe;
	};

	//bool Renderer::HasBoundingBoxEnabledImpl()
	MH_DEFINE_FUNC(Renderer::HasBoundingBoxEnabledImpl, bool)
	{
		return s_SceneData->BoundingBox;
	};

	//bool Renderer::HasGBufferEnabledImpl()
	MH_DEFINE_FUNC(Renderer::HasGBufferEnabledImpl, bool)
	{
		return s_SceneData->GBuffer;
	};

	//void Renderer::AddFrameBufferImpl(const std::string& name, WeakRef<FrameBuffer> frameBuffer)
	MH_DEFINE_FUNC(Renderer::AddFrameBufferImpl, void, const std::string& name, Asset<FrameBuffer> frameBuffer)
	{
		s_RendererData->FrameBuffers[name] = std::move(frameBuffer);
	};

	//const robin_hood::unordered_map<std::string, WeakRef<FrameBuffer>>& Renderer::GetFrameBuffersImpl()
	MH_DEFINE_FUNC(Renderer::GetFrameBuffersImpl, const Renderer::FrameBufferMap&)
	{
		auto iter = s_RendererData->FrameBuffers.begin();
		while (iter != s_RendererData->FrameBuffers.end())
		{
			if (iter->second.UseCount() == 1) // 2?
				iter = s_RendererData->FrameBuffers.erase(iter);
			else
				iter++;
		}

		return s_RendererData->FrameBuffers;
	};

	//Ref<FrameBuffer> Renderer::GetGBufferImpl()
	MH_DEFINE_FUNC(Renderer::GetGBufferImpl, Asset<FrameBuffer>)
	{
		return s_RendererData->GBuffer;
	};

	//Ref<FrameBuffer> Renderer::GetFrameBufferImpl()
	MH_DEFINE_FUNC(Renderer::GetFrameBufferImpl, Asset<FrameBuffer>)
	{
		return s_RendererData->ViewportFramebuffer;
	};

	//void Renderer::AddPerformanceResultImpl(uint32_t vertexCount, uint32_t indexCount)
	MH_DEFINE_FUNC(Renderer::AddPerformanceResultImpl, void, uint32_t vertexCount, uint32_t indexCount)
	{
#ifndef MH_STANDALONE
		s_RendererData->FrameResults.DrawCalls++;
		s_RendererData->FrameResults.VertexCount += vertexCount;
		s_RendererData->FrameResults.TriCount += indexCount;
#endif
	};

	//const RendererResults& Renderer::GetPerformanceResultsImpl()
	MH_DEFINE_FUNC(Renderer::GetPerformanceResultsImpl, const RendererResults&)
	{
		return s_RendererData->FrameResults;
	};
}