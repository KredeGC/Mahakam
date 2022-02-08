#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class LightingRenderPass : public RenderPass
	{
	private:
		Ref<Texture> brdfLut = nullptr;
		Ref<Texture> falloffLut = nullptr;
		Ref<Texture> spotlightTexture = nullptr;

		Ref<FrameBuffer> hdrFrameBuffer = nullptr;
		Ref<Shader> deferredShader = nullptr;

		Ref<FrameBuffer> shadowFramebuffer = nullptr;
		Ref<Shader> shadowShader = nullptr;

		Ref<UniformBuffer> shadowMatrixBuffer = nullptr;

		uint32_t shadowOffset;

	public:
		virtual void Init(uint32_t width, uint32_t height) override;
		virtual ~LightingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Ref<FrameBuffer>& src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() { return hdrFrameBuffer; }

	private:
		void RenderDirectionalShadows(SceneData* sceneData);

		void RenderDirectionalLights(SceneData* sceneData);
		void RenderPointLights(SceneData* sceneData);
		void RenderSpotLights(SceneData* sceneData);

		Ref<Texture> LoadOrCreateLUTTexture(const std::string& cachePath, const std::string& shaderPath, TextureFormat format, uint32_t width, uint32_t height);
	};
}