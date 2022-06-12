#include "mhpch.h"
#include "GeometryRenderPass.h"

#include "Mahakam/Core/Frustum.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	bool GeometryRenderPass::Init(uint32_t width, uint32_t height)
	{
		if (RenderPass::Init(width, height))
			return true;

		MH_PROFILE_RENDERING_FUNCTION();

		// Create gbuffer
		FrameBufferProps gProps;
		gProps.width = width;
		gProps.height = height;
		gProps.colorAttachments = {
			TextureFormat::RGBA8, // RGB - Albedo, A - Occlussion
			TextureFormat::RGBA8, // RG - Unused, B - Metallic, A - Roughness
			TextureFormat::RG11B10F, // RGB - Emission (not affected by light)
			TextureFormat::RGB10A2 }; // RGB - World normal, A - Unused
		gProps.depthAttachment = TextureFormat::Depth24; // Mutable

		gBuffer = FrameBuffer::Create(gProps);

		Renderer::AddFrameBuffer("GBuffer", gBuffer.Get());

		return true;
	}

	GeometryRenderPass::~GeometryRenderPass()
	{
		MH_PROFILE_FUNCTION();

		gBuffer = nullptr;
	}

	void GeometryRenderPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		gBuffer->Resize(width, height);
	}

	bool GeometryRenderPass::Render(SceneData* sceneData, Asset<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		GL::SetFillMode(!sceneData->wireframe);

		gBuffer->Bind();
		GL::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		GL::Clear();

		sceneData->cameraBuffer->Bind(0);

		// Create view projection frustum
		Frustum frustum(sceneData->cameraData.u_m4_VP);

		// Render all objects in queue
		uint16_t lastShaderID = ~0;
		uint16_t lastMaterialID = ~0;
		uint16_t lastMeshID = ~0;
		for (uint64_t drawID : sceneData->renderQueue)
		{
			const uint16_t passMask = (drawID >> 62ULL);
			if (passMask == 0ULL) // Opaque
			{
				// Choose a mesh
				const uint16_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
				Asset<Mesh>& mesh = sceneData->meshIDLookup[meshID];

				// Choose a transform
				const uint16_t transformID = drawID & 0xFFFFULL;
				const glm::mat4& transform = sceneData->transformIDLookup[transformID];

				// Perform AABB test
				const Mesh::Bounds transformedBounds = Mesh::TransformBounds(mesh->GetBounds(), transform);

				if (frustum.IsBoxVisible(transformedBounds.min, transformedBounds.max))
				{
					// Choose a shader
					const uint16_t shaderID = (drawID >> 47ULL) & 0x7FFFULL;
					if (shaderID != lastShaderID)
					{
						Asset<Shader>& shader = sceneData->shaderIDLookup[shaderID];
						if (!shader->HasShaderPass("GEOMETRY"))
							continue;
						lastShaderID = shaderID;
						shader->Bind("GEOMETRY"); // TODO: Some way of doing keyword support. Maybe baked into the drawID?
					}

					// Choose a material
					const uint16_t materialID = (drawID >> 32ULL) & 0x7FFFULL;
					Asset<Material>& material = sceneData->materialIDLookup[materialID];
					if (materialID != lastMaterialID)
					{
						lastMaterialID = materialID;
						material->Bind();
					}

					// Choose a mesh
					if (meshID != lastMeshID)
					{
						lastMeshID = meshID;
						mesh->Bind();
					}

					material->SetTransform(transform);

					Renderer::AddPerformanceResult(mesh->GetVertexCount(), mesh->GetIndexCount());

					GL::DrawIndexed(mesh->GetIndexCount());
				}
			}
		}

		gBuffer->Unbind();

		GL::SetFillMode(true);

		return true;
	}
}