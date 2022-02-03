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
		Ref<Material> deferredMaterial;

		bool updateTextures = true;

	public:
		LightingRenderPass(uint32_t width, uint32_t height);
		virtual ~LightingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual void Render(Renderer::SceneData* sceneData, Ref<FrameBuffer>& src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() { return hdrFrameBuffer; }

	private:
		void RenderDirectionalLights(Renderer::SceneData* sceneData);
		void RenderPointLights(Renderer::SceneData* sceneData);
		void RenderSpotLights(Renderer::SceneData* sceneData);

		Ref<Texture> LoadOrCreateLUTTexture(const std::string& cachePath, const std::string& shaderPath, TextureFormat format, uint32_t width, uint32_t height);
	};
}