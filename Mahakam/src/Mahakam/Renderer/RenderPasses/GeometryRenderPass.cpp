#include "Mahakam/mhpch.h"
#include "GeometryRenderPass.h"

#include "Mahakam/Core/Frustum.h"

#include "Mahakam/Math/Bounds.h"

#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/RenderData.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/Shader.h"

namespace Mahakam
{
	bool GeometryRenderPass::Init(uint32_t width, uint32_t height)
	{
		if (RenderPass::Init(width, height))
			return true;

		MH_PROFILE_RENDERING_FUNCTION();

		// Create gbuffer
		FrameBufferProps gProps;
		gProps.Width = width;
		gProps.Height = height;
		gProps.ColorAttachments = TrivialVector<FrameBufferAttachmentProps>{
			TextureFormat::RGBA8, // RGB - Albedo, A - Occlussion
			TextureFormat::RGBA8, // RG - Unused, B - Metallic, A - Roughness
			TextureFormat::RG11B10F, // RGB - Emission (not affected by light)
			TextureFormat::RGB10A2 }; // RGB - World normal, A - Unused
		gProps.DepthAttachment = TextureFormat::Depth24; // Mutable

		gBuffer = FrameBuffer::Create(gProps);

		Renderer::AddFrameBuffer("GBuffer", gBuffer);

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

	bool GeometryRenderPass::Render(SceneData* sceneData, const Asset<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		GL::SetFillMode(!sceneData->Wireframe);

		gBuffer->Bind();
		GL::SetClearColor({ 1.0f, 0.06f, 0.94f, 1.0f });
		GL::Clear();

		sceneData->CameraBuffer->Bind(0);

		// Create view projection frustum
		Frustum frustum(sceneData->CameraMatrix.u_m4_VP);

		// Render all objects in queue
		uint16_t lastShaderID = ~0;
		uint16_t lastMaterialID = ~0;
		uint16_t lastMeshID = ~0;
		for (uint64_t drawID : sceneData->RenderQueue)
		{
			const uint16_t passMask = (drawID >> 62ULL);
			if (passMask == 0ULL) // Opaque
			{
				// Choose a mesh
				const uint16_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
				Ref<SubMesh>& mesh = sceneData->MeshIDLookup[meshID];

				// Choose a transform
				const uint16_t transformID = drawID & 0xFFFFULL;
				const glm::mat4& transform = sceneData->TransformIDLookup[transformID];

				// Perform AABB test
				const Bounds transformedBounds = Bounds::TransformBounds(mesh->GetBounds(), transform);

				if (frustum.IsBoxVisible(transformedBounds.Min, transformedBounds.Max))
				{
					// Choose a shader
					const uint16_t shaderID = (drawID >> 47ULL) & 0x7FFFULL;
					if (shaderID != lastShaderID)
					{
						Asset<Shader>& shader = sceneData->ShaderIDLookup[shaderID];
						if (!shader->HasShaderPass("GEOMETRY"))
							continue;
						lastShaderID = shaderID;
						shader->Bind("GEOMETRY");
					}

					// Choose a material
					const uint16_t materialID = (drawID >> 32ULL) & 0x7FFFULL;
					Asset<Material>& material = sceneData->MaterialIDLookup[materialID];
					if (materialID != lastMaterialID)
					{
						lastMaterialID = materialID;
						material->Bind(*sceneData->UniformValueBuffer);
					}

					// Choose a mesh
					if (meshID != lastMeshID)
					{
						lastMeshID = meshID;
						mesh->Bind();
					}

					sceneData->CameraBuffer->SetData(&transform, 0, sizeof(glm::mat4));

					Renderer::AddPerformanceResult(mesh->GetVertexCount(), mesh->GetIndexCount());

					GL::DrawIndexed(mesh->GetIndexCount());
				}
			}
		}

		// Render skybox
		if (!sceneData->GBuffer && !sceneData->Wireframe)
		{
			GL::EnableZWriting(false);

			Renderer::DrawSkybox(sceneData);

			GL::EnableZWriting(true);
		}

		gBuffer->Unbind();

		if (sceneData->Wireframe)
			GL::SetFillMode(true);

		return true;
	}
}