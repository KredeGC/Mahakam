#include "mhpch.h"
#include "ParticleRenderPass.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Renderer.h"

namespace Mahakam
{
	bool ParticleRenderPass::Init(uint32_t width, uint32_t height)
	{
		if (RenderPass::Init(width, height))
			return true;

		// Create viewport framebuffer
		FrameBufferProps viewportProps;
		viewportProps.width = width;
		viewportProps.height = height;
		viewportProps.colorAttachments = { TextureFormat::RG11B10F };

		viewportFramebuffer = FrameBuffer::Create(viewportProps);

		particleCompute = ComputeShader::Create("assets/compute/Particles.glsl");

		return true;
	}

	ParticleRenderPass::~ParticleRenderPass()
	{
		viewportFramebuffer = nullptr;
		particleCompute = nullptr;
	}

	void ParticleRenderPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		viewportFramebuffer->Resize(width, height);
	}

	bool ParticleRenderPass::Render(SceneData* sceneData, Ref<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		GL::SetFillMode(!sceneData->wireframe);

		src->Blit(viewportFramebuffer, true, true);

		viewportFramebuffer->Bind();

		Ref<FrameBuffer> gBuffer = Renderer::GetGBuffer();
		gBuffer->GetColorTexture(3)->Bind(0);
		gBuffer->GetDepthTexture()->Bind(1);

		// Render all particles in queue
		uint16_t lastShaderID = ~0;
		uint16_t lastMaterialID = ~0;
		uint16_t lastMeshID = ~0;
		for (uint64_t drawID : sceneData->renderQueue)
		{
			// Choose a particle system
			const uint16_t particleID = (drawID >> 16ULL) & 0xFFFFULL;
			ParticleSystem& particles = sceneData->particleIDLookup[particleID];

			// Choose a transform
			const uint16_t transformID = drawID & 0xFFFFULL;
			const glm::mat4& transform = sceneData->transformIDLookup[transformID];

			particles.BindBuffers(transform);

			particles.Simulate(sceneData->deltaTime);

			//Renderer::AddPerformanceResult(6, 6);

			//GL::DrawIndirect();
		}

		viewportFramebuffer->Unbind();

		GL::SetFillMode(true);

		return true;
	}
}