#include "Mahakam/mhpch.h"
#include "ParticleRenderPass.h"

#include "Mahakam/Renderer/ComputeShader.h"
#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/RenderData.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	bool ParticleRenderPass::Init(uint32_t width, uint32_t height)
	{
		if (RenderPass::Init(width, height))
			return true;

		MH_PROFILE_RENDERING_FUNCTION();

		particleCompute = ComputeShader::Create("resource/compute/Particles.glsl");

		return true;
	}

	ParticleRenderPass::~ParticleRenderPass()
	{
		MH_PROFILE_FUNCTION();

		particleCompute = nullptr;
	}

	bool ParticleRenderPass::Render(SceneData* sceneData, const Asset<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		GL::SetFillMode(!sceneData->Wireframe);

		src->Bind();

		Asset<FrameBuffer> gBuffer = Renderer::GetGBuffer();
		gBuffer->GetColorTexture(3)->Bind(0);
		gBuffer->GetDepthTexture()->Bind(1);

		// Render all particles in queue
		uint16_t lastShaderID = ~0;
		uint16_t lastMaterialID = ~0;
		uint16_t lastMeshID = ~0;
		for (uint64_t drawID : sceneData->RenderQueue)
		{
			// Choose a particle system
			const uint16_t particleID = (drawID >> 16ULL) & 0xFFFFULL;
			ParticleSystem& particles = sceneData->ParticleIDLookup[particleID];

			// Choose a transform
			const uint16_t transformID = drawID & 0xFFFFULL;
			const glm::mat4& transform = sceneData->TransformIDLookup[transformID];

			particles.BindBuffers(transform);

			particles.Simulate(sceneData->DeltaTime);

			//Renderer::AddPerformanceResult(6, 6);

			//GL::DrawIndirect();
		}

		src->Unbind();

		GL::SetFillMode(true);

		return false;
	}
}