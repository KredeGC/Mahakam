#pragma once

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class BoundingBoxRenderPass : public RenderPass
	{
	protected:
		Asset<Shader> m_UnlitShader;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~BoundingBoxRenderPass() override;

		virtual bool Render(SceneData* sceneData, const Asset<FrameBuffer>& src) override;

		virtual Asset<FrameBuffer> GetFrameBuffer() override { return nullptr; };
	};
}