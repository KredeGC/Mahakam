#include "ebpch.h"
#include "BoundingBoxRenderPass.h"

namespace Mahakam::Editor
{
	bool BoundingBoxRenderPass::Init(uint32_t width, uint32_t height)
	{
		if (RenderPass::Init(width, height))
			return true;

		MH_PROFILE_RENDERING_FUNCTION();

		m_UnlitShader = Shader::Create("internal/shaders/builtin/Wireframe.shader");

		return true;
	}

	BoundingBoxRenderPass::~BoundingBoxRenderPass()
	{
		MH_PROFILE_FUNCTION();
	}

	bool BoundingBoxRenderPass::Render(SceneData* sceneData, const Asset<FrameBuffer>& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (!sceneData->boundingBox)
			return false;

		src->Bind();

		GL::SetFillMode(false);
		GL::EnableCulling(false);

		m_UnlitShader->Bind("POSTPROCESSING");

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

			sceneData->cameraBuffer->SetData(&wireTransform, 0, sizeof(glm::mat4));

			Renderer::AddPerformanceResult(wireMesh->GetVertexCount(), wireMesh->GetIndexCount());

			GL::DrawIndexed(wireMesh->GetIndexCount());
		}

		src->Unbind();

		GL::EnableCulling(true);
		GL::SetFillMode(true);

		return false;
	}
}