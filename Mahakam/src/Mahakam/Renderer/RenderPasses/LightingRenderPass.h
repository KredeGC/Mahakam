#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class LightingRenderPass : public RenderPass
	{
	private:
		Ref<Texture> brdfLut;
		Ref<Texture> falloffLut;
		Ref<Texture> spotlightTexture;

		Ref<FrameBuffer> hdrFrameBuffer;
		Ref<Shader> deferredShader;

	public:
		virtual void Init(uint32_t width, uint32_t height) override;
		virtual ~LightingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Ref<FrameBuffer>& src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() { return hdrFrameBuffer; }

	private:
		void RenderDirectionalLights(SceneData* sceneData);
		void RenderPointLights(SceneData* sceneData);
		void RenderSpotLights(SceneData* sceneData);

		Ref<Texture> LoadOrCreateLUTTexture(const std::string& cachePath, const std::string& shaderPath, TextureFormat format, uint32_t width, uint32_t height);
	};
}