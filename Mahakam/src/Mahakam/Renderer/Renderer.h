#pragma once

#include "FrameBuffer.h"

#include "Camera.h"
#include "Mesh.h"
#include "Light.h"

#include <vector>

namespace Mahakam
{
	class Renderer
	{
	public:
		struct DirectionalLight
		{
		public:
			glm::vec3 direction;
		private:
			float padding01 = 0.0f;
		public:
			glm::vec3 color;
		private:
			float padding02 = 0.0f;

		public:
			DirectionalLight(const glm::vec3& direction, const glm::vec3& color)
				: direction(direction), color(color) {}
		};

		struct PointLight
		{
		public:
			glm::vec4 position; // w - range
			glm::vec4 color; // w - 1.0 / (range * range)

		public:
			PointLight(const glm::vec3& position, float range, const glm::vec3& color) :
				position(glm::vec4(position, range)),
				color(color, 1.0f / (range * range)) {}
		};

		struct SpotLight
		{
		public:
			glm::mat4 objectToWorld;
			glm::mat4 worldToLight;
			glm::vec4 color; // w - 1.0 / (range * range)

		public:
			SpotLight(const glm::vec3& position, const glm::quat& rotation, float fov, float range, const glm::vec3& color) :
				worldToLight(glm::perspective(fov, 1.0f, 0.03f, range)* glm::inverse(glm::translate(glm::mat4(1.0f), position)* glm::mat4(rotation))),
				color(color, 1.0f / (range * range))
			{
				float xy = glm::tan(fov / 2.0f) * range;
				objectToWorld = glm::translate(glm::mat4(1.0f), position)
					* glm::mat4(rotation)
					* glm::scale(glm::mat4(1.0f), glm::vec3(xy, xy, range));
			}
		};

		struct EnvironmentData
		{
			Ref<Material> skyboxMaterial;
			Ref<Texture> irradianceMap;
			Ref<Texture> specularMap;
			std::vector<DirectionalLight> directionalLights;
			std::vector<PointLight> pointLights;
			std::vector<SpotLight> spotLights;
		};

		struct RendererResults
		{
			uint32_t drawCalls = 0;
			uint32_t vertexCount = 0;
			uint32_t triCount = 0;
		};

	private:
		struct SceneData
		{
			bool wireframe = false;

			// Environment data, provided by scene
			EnvironmentData environment;

			// Render camera matrices
			Ref<UniformBuffer> cameraBuffer;

			// Deferred lighting
			Ref<FrameBuffer> gBuffer;
			Ref<FrameBuffer> hdrFrameBuffer;
			Ref<FrameBuffer> viewportFramebuffer;

			// Lighting buffers
			Ref<StorageBuffer> directionalLightBuffer;
			Ref<StorageBuffer> pointLightBuffer;
			Ref<StorageBuffer> spotLightBuffer;

			// Materials
			Ref<Material> deferredMaterial;
			Ref<Material> tonemappingMaterial;
		};

		struct MeshData
		{
			float depth;
			Ref<Mesh> mesh;
			Ref<Material> material;
			glm::mat4 transform;
		};

		struct CameraData
		{
		public:
			// View & projection matrices
			glm::mat4 u_m4_V;
			glm::mat4 u_m4_P;

			// Inverse view & projection matrices
			glm::mat4 u_m4_IV;
			glm::mat4 u_m4_IP;

			// View-projection matrices
			glm::mat4 u_m4_VP;
			glm::mat4 u_m4_IVP;

			// Camera position
			glm::vec3 u_CameraPos;

		private:
			float padding01 = 0.0f;

		public:
			CameraData(const Camera& camera, const glm::mat4& transform) :
				u_m4_V(glm::inverse(transform)),
				u_m4_P(camera.getProjectionMatrix()),
				u_m4_IV(transform),
				u_m4_IP(glm::inverse(camera.getProjectionMatrix())),
				u_m4_VP(u_m4_P* u_m4_V),
				u_m4_IVP(u_m4_IV* u_m4_IP),
				u_CameraPos(transform[3]) {}
		};

		static RendererResults* rendererResults;
		static SceneData* sceneData;

		static std::unordered_map<Ref<Shader>, // Sort by shader
			std::unordered_map<Ref<Material>, // Sort by material
			std::unordered_map<Ref<Mesh>, // Sort by mesh
			std::vector<glm::mat4>>>> renderQueue; // List of transforms for this mesh

		static std::vector<MeshData> transparentQueue;

	public:
		static void init(uint32_t width, uint32_t height);
		static void onWindowResie(uint32_t width, uint32_t height);

		static void beginScene(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment);
		static void endScene();

		static void enableWireframe(bool enable);

		static void submit(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material);
		static void submitTransparent(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material);

		inline static Ref<FrameBuffer> getGBuffer() { return sceneData->gBuffer; }
		inline static Ref<FrameBuffer> getFrameBuffer() { return sceneData->viewportFramebuffer; }

		inline static const RendererResults* getPerformanceResults() { return rendererResults; }

	private:
		static void drawOpaqueQueue();
		static void drawTransparentQueue();

		static void drawSkybox();
		static void drawScreenQuad();
		static void drawInstancedSphere(uint32_t amount);
		static void drawInstancedPyramid(uint32_t amount);

		static void renderGeometryPass();
		static void renderLightingPass();
		static void renderFinalPass();
	};
}