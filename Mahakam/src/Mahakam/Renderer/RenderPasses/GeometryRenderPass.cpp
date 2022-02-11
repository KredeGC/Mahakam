#include "mhpch.h"
#include "GeometryRenderPass.h"

#include "Mahakam/Core/Frustum.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Renderer.h"

namespace Mahakam
{
	void GeometryRenderPass::Init(uint32_t width, uint32_t height)
	{
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

		gBuffer = FrameBuffer::Create(gProps);
	}

	GeometryRenderPass::~GeometryRenderPass()
	{
		gBuffer = nullptr;
	}

	void GeometryRenderPass::OnWindowResize(uint32_t width, uint32_t height)
	{
		gBuffer->Resize(width, height);
	}

	bool GeometryRenderPass::Render(SceneData* sceneData, Ref<FrameBuffer>& src)
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
		uint64_t lastShaderID = ~0;
		uint64_t lastMaterialID = ~0;
		uint64_t lastMeshID = ~0;
		for (uint64_t drawID : sceneData->renderQueue)
		{
			const uint64_t passMask = (drawID >> 62ULL);
			if (passMask == 0ULL) // Opaque
			{
				// Choose a shader
				const uint64_t shaderID = (drawID >> 47ULL) & 0x7FFFULL;
				if (shaderID != lastShaderID)
				{
					Ref<Shader>& shader = sceneData->shaderIDLookup[shaderID];
					if (!shader->HasShaderPass("GEOMETRY"))
						continue;
					lastShaderID = shaderID;
					shader->Bind("GEOMETRY"); // TODO: Some way of doing keyword support. Maybe baked into the ID?
				}

				// Choose a material
				const uint64_t materialID = (drawID >> 32ULL) & 0x7FFFULL;
				Ref<Material>& material = sceneData->materialIDLookup[materialID];
				if (materialID != lastMaterialID)
				{
					lastMaterialID = materialID;
					material->Bind();
				}

				// Choose a mesh
				const uint64_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
				Ref<Mesh>& mesh = sceneData->meshIDLookup[meshID];
				if (meshID != lastMeshID)
				{
					lastMeshID = meshID;
					mesh->Bind();
				}

				// Choose a transform
				const uint64_t transformID = drawID & 0xFFFFULL;
				glm::mat4& transform = sceneData->transformIDLookup[transformID];

				// Perform AABB test
				Mesh::Bounds transformedBounds = Mesh::TransformBounds(mesh->GetBounds(), transform);

				if (frustum.IsBoxVisible(transformedBounds.min, transformedBounds.max))
				{
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