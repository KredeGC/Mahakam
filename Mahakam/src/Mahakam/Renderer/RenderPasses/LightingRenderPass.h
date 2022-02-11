#pragma once

#include "RenderPass.h"

#include <glm/gtx/hash.hpp>
#include <robin_hood.h>

namespace Mahakam
{
	class Frustum;

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

		robin_hood::unordered_map<glm::ivec2, uint64_t> lightHashes;

		glm::ivec2 shadowMapOffset = { 0.0f, 0.0f };
		glm::ivec2 shadowMapMargin = { 0.0f, 0.0f };
		static constexpr uint32_t shadowMapSize = 8192;

	public:
		virtual void Init(uint32_t width, uint32_t height) override;
		virtual ~LightingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Ref<FrameBuffer>& src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() { return hdrFrameBuffer; }

	private:
		uint64_t PrePassShadowGeometry(SceneData* sceneData, const Frustum& frustum);
		void RenderShadowGeometry(SceneData* sceneData, const Frustum& frustum, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID);

		void RenderDirectionalShadows(SceneData* sceneData, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID);
		void RenderSpotShadows(SceneData* sceneData, const Frustum& cameraFrustum, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID);

		void RenderDirectionalLights(SceneData* sceneData);
		void RenderPointLights(SceneData* sceneData);
		void RenderSpotLights(SceneData* sceneData);

		Ref<Texture> LoadOrCreateLUTTexture(const std::string& cachePath, const std::string& shaderPath, TextureFormat format, uint32_t width, uint32_t height);
	};
}