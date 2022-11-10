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

	class LightingRenderPass : public RenderPass
	{
	protected:
		Asset<Texture> m_BRDFLut = nullptr;
		Asset<Texture> m_FalloffLut = nullptr;
		Asset<Texture> m_SpotlightTexture = nullptr;

		Asset<FrameBuffer> m_HDRFrameBuffer = nullptr;
		Asset<Shader> m_DeferredShader = nullptr;

		Asset<FrameBuffer> m_ShadowFramebuffer = nullptr;
		Asset<Shader> m_ShadowShader = nullptr;

		Ref<UniformBuffer> m_ShadowMatrixBuffer = nullptr;

		UnorderedMap<glm::ivec2, uint64_t> m_LightHashes;

		TrivialVector<uint64_t> m_RenderQueue;

		glm::ivec2 m_ShadowMapOffset = { 0.0f, 0.0f };
		glm::ivec2 m_ShadowMapMargin = { 0.0f, 0.0f };

		static constexpr uint32_t s_ShadowMapSize = 8192;
		static constexpr uint32_t s_DirectionalShadowSize = 4096;
		static constexpr uint32_t s_SpotShadowSize = 512;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~LightingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, const Asset<FrameBuffer>& src) override;

		virtual Asset<FrameBuffer> GetFrameBuffer() override { return m_HDRFrameBuffer; };

	protected:
		virtual void SetupBRDF();
		virtual void SetupFrameBuffer(uint32_t width, uint32_t height);
		virtual void SetupShaders();

		virtual void RenderShadowMaps(SceneData* sceneData, const Frustum& frustum);
		virtual void SetupTextures(SceneData* sceneData, const Asset<FrameBuffer>& src);
		virtual void RenderLighting(SceneData* sceneData, const Asset<FrameBuffer>& src);

		virtual uint64_t PrePassShadowGeometry(SceneData* sceneData, const Frustum& frustum);
		virtual void RenderShadowGeometry(SceneData* sceneData, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID);

		virtual void RenderDirectionalShadows(SceneData* sceneData, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID);
		virtual void RenderSpotShadows(SceneData* sceneData, const Frustum& cameraFrustum, uint64_t* lastShaderID, uint64_t* lastMaterialID, uint64_t* lastMeshID);

		virtual void RenderDirectionalLights(SceneData* sceneData);
		virtual void RenderPointLights(SceneData* sceneData);
		virtual void RenderSpotLights(SceneData* sceneData);

		virtual Asset<Texture> LoadOrCreateLUTTexture(const std::string& cachePath, const std::string& shaderPath, TextureFormat format, uint32_t width, uint32_t height);
	};
}