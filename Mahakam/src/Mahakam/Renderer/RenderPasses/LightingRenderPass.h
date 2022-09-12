#pragma once

#include "RenderPass.h"

#include "Mahakam/Renderer/TextureFormats.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Mahakam
{
	class Frustum;
	class Shader;
	class Texture;
	class UniformBuffer;

	extern template class Asset<Shader>;
	extern template class Asset<Texture>;

	class LightingRenderPass : public RenderPass
	{
	protected:
		Ref<Texture> brdfLut = nullptr;
		Ref<Texture> falloffLut = nullptr;
		Ref<Texture> spotlightTexture = nullptr;

		Ref<FrameBuffer> hdrFrameBuffer = nullptr;
		Ref<Shader> deferredShader = nullptr;

		Ref<FrameBuffer> shadowFramebuffer = nullptr;
		Ref<Shader> shadowShader = nullptr;

		Ref<UniformBuffer> shadowMatrixBuffer = nullptr;

		UnorderedMap<glm::ivec2, uint64_t> lightHashes;

		glm::ivec2 shadowMapOffset = { 0.0f, 0.0f };
		glm::ivec2 shadowMapMargin = { 0.0f, 0.0f };

		static constexpr uint32_t s_ShadowMapSize = 8192;
		static constexpr uint32_t s_DirectionalShadowSize = 4096;
		static constexpr uint32_t s_SpotShadowSize = 512;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~LightingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Ref<FrameBuffer> src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() override { return hdrFrameBuffer; };

	protected:
		virtual void SetupBRDF();
		virtual void SetupFrameBuffer(uint32_t width, uint32_t height);
		virtual void SetupShaders();

		virtual void RenderShadowMaps(SceneData* sceneData, const Frustum& frustum);
		virtual void SetupTextures(SceneData* sceneData, Ref<FrameBuffer> src);
		virtual void RenderLighting(SceneData* sceneData, Ref<FrameBuffer> src);

		virtual uint64_t PrePassShadowGeometry(SceneData* sceneData, const Frustum& frustum, std::vector<uint64_t>& renderQueue);
		virtual void RenderShadowGeometry(SceneData* sceneData, const Frustum& frustum, const std::vector<uint64_t>& renderQueue, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID);

		virtual void RenderDirectionalShadows(SceneData* sceneData, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID);
		virtual void RenderSpotShadows(SceneData* sceneData, const Frustum& cameraFrustum, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID);

		virtual void RenderDirectionalLights(SceneData* sceneData);
		virtual void RenderPointLights(SceneData* sceneData);
		virtual void RenderSpotLights(SceneData* sceneData);

		virtual Ref<Texture> LoadOrCreateLUTTexture(const std::string& cachePath, const std::string& shaderPath, TextureFormat format, uint32_t width, uint32_t height);
	};
}