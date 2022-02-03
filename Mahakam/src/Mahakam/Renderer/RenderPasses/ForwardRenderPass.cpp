#include "mhpch.h"
#include "ForwardRenderPass.h"

#include "Mahakam/Renderer/GL.h"

namespace Mahakam
{
	ForwardRenderPass::ForwardRenderPass(uint32_t width, uint32_t height)
	{
		// Create gbuffer
		FrameBufferProps gProps;
		gProps.width = width;
		gProps.height = height;
		gProps.colorAttachments = { TextureFormat::RGBA8 };

		forwardFramebuffer = FrameBuffer::Create(gProps);
	}

	ForwardRenderPass::~ForwardRenderPass()
	{
		forwardFramebuffer = nullptr;
	}

	void ForwardRenderPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		forwardFramebuffer->Resize(width, height);
	}

	void ForwardRenderPass::Render(Renderer::SceneData* sceneData, Ref<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (sceneData->wireframe)
		{
			src->Blit(forwardFramebuffer, true, false);
			return;
		}

		src->Blit(forwardFramebuffer);

		forwardFramebuffer->Bind();

		sceneData->cameraBuffer->Bind(0);

		GL::SetBlendMode(RendererAPI::BlendMode::SrcAlpha, RendererAPI::BlendMode::OneMinusSrcAlpha, true);

		// Render all objects in queue
		uint64_t lastShaderID = ~0;
		uint64_t lastMaterialID = ~0;
		uint64_t lastMeshID = ~0;
		for (uint64_t drawID : sceneData->renderQueue)
		{
			const uint64_t passMask = (drawID >> 62ULL);
			if (passMask == 2) // Transparent
			{
				const uint64_t shaderID = (drawID >> 47ULL) & 0x7FFFULL;
				if (shaderID != lastShaderID)
				{
					Ref<Shader>& shader = sceneData->shaderIDLookup[shaderID];
					if (!shader->HasShaderPass("FORWARD"))
						continue;
					lastShaderID = shaderID;
					shader->Bind("FORWARD"); // TODO: Some way of doing keyword support. Maybe baked into the ID?
				}

				const uint64_t materialID = (drawID >> 32ULL) & 0x7FFFULL;
				Ref<Material>& material = sceneData->materialIDLookup[materialID];
				if (materialID != lastMaterialID)
				{
					lastMaterialID = materialID;
					material->Bind();
				}

				const uint64_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
				Ref<Mesh>& mesh = sceneData->meshIDLookup[meshID];
				if (meshID != lastMeshID)
				{
					lastMeshID = meshID;
					mesh->Bind();
				}

				const uint64_t transformID = drawID & 0xFFFFULL;
				glm::mat4& transform = sceneData->transformIDLookup[transformID];

				material->SetTransform(transform);

				Renderer::AddPerformanceResult(mesh->GetVertexCount(), mesh->GetIndexCount());

				GL::DrawIndexed(mesh->GetIndexCount());
			}
		}

		GL::SetBlendMode(RendererAPI::BlendMode::One, RendererAPI::BlendMode::One, false);

		forwardFramebuffer->Unbind();
	}
}