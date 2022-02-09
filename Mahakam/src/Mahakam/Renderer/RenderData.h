#pragma once

#include <glm/glm.hpp>

#include <glm/gtx/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <vector>

#include <robin_hood.h>

namespace Mahakam
{
	// Forward declare most things, don't want to pollute the header files too much
	class Camera;
	class Mesh;
	class Light;
	class Shader;
	class Material;
	class UniformBuffer;
	class StorageBuffer;
	class Texture;

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
		glm::mat4 worldToLight;
		glm::vec4 offset = { 0.0f, 0.0f, 0.0f, 0.0f }; // xy - offset, z - size, w - bias

	public:
		DirectionalLight(const glm::vec3& position, const glm::quat& rotation, const Light& light);
	};

	struct PointLight
	{
	public:
		glm::vec4 position; // w - range
		glm::vec4 color; // w - 1.0 / (range * range)

	public:
		PointLight(const glm::vec3& position, const Light& light);
	};

	struct SpotLight
	{
	public:
		glm::mat4 objectToWorld;
		glm::vec4 color; // w - 1.0 / (range * range)
		glm::mat4 worldToLight;
		glm::vec4 offset = { 0.0f, 0.0f, 0.0f, 0.0f }; // xy - offset, z - size, w - bias

	public:
		SpotLight(const glm::vec3& position, const glm::quat& rotation, const Light& light);
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

		CameraData(const Camera& camera, const glm::mat4& transform);

		CameraData(const glm::mat4& projectionMatrix, const glm::mat4& transform);
	};

	struct SceneData
	{
		bool wireframe = false;
		bool boundingBox = false;
		bool gBuffer = false;

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
}