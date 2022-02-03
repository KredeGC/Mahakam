#pragma once

#include "FrameBuffer.h"

#include "Camera.h"
#include "Mesh.h"
#include "Light.h"

#include <vector>

#include <robin_hood.h>

namespace Mahakam
{
	class RenderPass;

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
			glm::mat4 worldToLight;

		public:
			DirectionalLight(const glm::vec3& direction, const Light& light) :
				direction(direction), color(light.GetColor()),
				worldToLight(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.03f, 20.0f)* glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
					glm::vec3(0.0f, 0.0f, 0.0f),
					glm::vec3(0.0f, 1.0f, 0.0f))) {}
		};

		struct PointLight
		{
		public:
			glm::vec4 position; // w - range
			glm::vec4 color; // w - 1.0 / (range * range)

		public:
			PointLight(const glm::vec3& position, const Light& light) :
				position(glm::vec4(position, light.GetRange())),
				color(light.GetColor(), 1.0f / (light.GetRange() * light.GetRange())) {}
		};

		struct SpotLight
		{
		public:
			glm::mat4 objectToWorld;
			glm::mat4 worldToLight;
			glm::vec4 color; // w - 1.0 / (range * range)

		public:
			SpotLight(const glm::vec3& position, const glm::quat& rotation, const Light& light) :
				worldToLight(glm::perspective(light.GetFov(), 1.0f, 0.03f, light.GetRange())* glm::inverse(glm::translate(glm::mat4(1.0f), position)* glm::mat4(rotation))),
				color(light.GetColor(), 1.0f / (light.GetRange() * light.GetRange()))
			{
				float xy = glm::tan(light.GetFov() / 2.0f) * light.GetRange();
				objectToWorld = glm::translate(glm::mat4(1.0f), position)
					* glm::mat4(rotation)
					* glm::scale(glm::mat4(1.0f), glm::vec3(xy, xy, light.GetRange()));
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
			CameraData() = default;

			CameraData(const Camera& camera, const glm::mat4& transform) :
				u_m4_V(glm::inverse(transform)),
				u_m4_P(camera.GetProjectionMatrix()),
				u_m4_IV(transform),
				u_m4_IP(glm::inverse(camera.GetProjectionMatrix())),
				u_m4_VP(u_m4_P* u_m4_V),
				u_m4_IVP(u_m4_IV* u_m4_IP),
				u_CameraPos(transform[3]) {}
		};

		struct SceneData
		{
			bool wireframe = false;

			// Render queue
			// 64 bit render queue ID
			// 2 bit - Geometry(0), AlphaTest(1), Transparent(2), Fullscreen(3)
			// If Opaque:
			//  15 bits - Shader index
			//  15 bits - Material index
			//  16 bits - Mesh index
			//  16 bits - Transform index
			// If Transparent:
			//  32 bits - Depth
			std::vector<uint64_t> renderQueue;

			robin_hood::unordered_map<Ref<Shader>, uint64_t> shaderRefLookup;
			robin_hood::unordered_map<Ref<Material>, uint64_t> materialRefLookup;
			robin_hood::unordered_map<Ref<Mesh>, uint64_t> meshRefLookup;

			robin_hood::unordered_map<uint64_t, Ref<Shader>> shaderIDLookup;
			robin_hood::unordered_map<uint64_t, Ref<Material>> materialIDLookup;
			robin_hood::unordered_map<uint64_t, Ref<Mesh>> meshIDLookup;
			robin_hood::unordered_map<uint64_t, glm::mat4> transformIDLookup;

			// Environment data, provided by scene
			EnvironmentData environment;

			// Render camera matrices
			CameraData cameraData;
			Ref<UniformBuffer> cameraBuffer;

			// Lighting buffers
			Ref<StorageBuffer> directionalLightBuffer;
			Ref<StorageBuffer> pointLightBuffer;
			Ref<StorageBuffer> spotLightBuffer;
		};

	private:
		struct MeshData
		{
			float depth;
			Ref<Mesh> mesh;
			Ref<Material> material;
			glm::mat4 transform;
		};

		static RendererResults* rendererResults;
		static SceneData* sceneData;
		static std::vector<RenderPass*> renderPasses;
		static Ref<FrameBuffer> viewportFramebuffer;

	public:
		static void Init(uint32_t width, uint32_t height);
		static void Shutdown();
		static void OnWindowResie(uint32_t width, uint32_t height);

		static void BeginScene(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment);
		static void EndScene();

		static void EnableWireframe(bool enable);

		static void Submit(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material);

		static void DrawSkybox();
		static void DrawScreenQuad();
		static void DrawInstancedSphere(uint32_t amount);
		static void DrawInstancedPyramid(uint32_t amount);

		inline static Ref<FrameBuffer> GetFrameBuffer() { return viewportFramebuffer; }

		inline static void AddPerformanceResult(uint32_t vertexCount, uint32_t indexCount)
		{
			rendererResults->drawCalls++;
			rendererResults->vertexCount += vertexCount;
			rendererResults->triCount += indexCount;
		}

		inline static const RendererResults* GetPerformanceResults() { return rendererResults; }
	};
}