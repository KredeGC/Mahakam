#pragma once

#include "Mahakam/Core/Core.h"

#include "ParticleSystem.h"

#include <glm/glm.hpp>

#include <vector>

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

		uint64_t Hash();
	};

	struct PointLight
	{
	public:
		glm::vec4 position; // w - range
		glm::vec4 color; // w - 1.0 / (range * range)

	public:
		PointLight(const glm::vec3& position, const Light& light);

		uint64_t Hash();
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

		uint64_t Hash();
	};

	struct EnvironmentData
	{
		Asset<Material> skyboxMaterial;
		Asset<Texture> irradianceMap;
		Asset<Texture> specularMap;
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
		glm::vec4 u_ScreenParams;

	public:
		CameraData() = default;

		CameraData(const Camera& camera, const glm::vec2& screenSize, const glm::mat4& transform);

		CameraData(const glm::mat4& projectionMatrix, const glm::vec2& screenSize, const glm::mat4& transform);
	};

	struct SceneData
	{
		bool wireframe = false;
		bool boundingBox = false;
		bool gBuffer = false;

		float deltaTime;

		// Render queue ID
		// 2 bits - Geometry(0), AlphaTest(1), Transparent(2), Fullscreen(3)
		// If Opaque:
		//  15 bits - Shader index
		//  15 bits - Material index
		//  16 bits - Mesh index
		//  16 bits - Transform index
		// If Transparent:
		//  30 bits - Depth
		//  16 bits - Mesh index
		//  16 bits - Transform index
		std::vector<uint64_t> renderQueue;

		UnorderedMap<Asset<Shader>, uint64_t> shaderRefLookup;
		UnorderedMap<Asset<Material>, uint64_t> materialRefLookup;
		UnorderedMap<Asset<Mesh>, uint64_t> meshRefLookup;

		UnorderedMap<uint64_t, Asset<Shader>> shaderIDLookup;
		UnorderedMap<uint64_t, Asset<Material>> materialIDLookup;
		UnorderedMap<uint64_t, Asset<Mesh>> meshIDLookup;
		UnorderedMap<uint64_t, glm::mat4> transformIDLookup;

		// Particle queue ID
		// 32 bits - Depth
		// 16 bits - ParticleSystem index
		// 16 bits - Transform index
		std::vector<uint64_t> particleQueue;

		UnorderedMap<uint64_t, ParticleSystem> particleIDLookup;

		// Environment data, provided by scene
		EnvironmentData environment;

		// Render camera matrices
		CameraData cameraData;
		Asset<UniformBuffer> cameraBuffer;

		// Lighting buffers
		Asset<StorageBuffer> directionalLightBuffer;
		Asset<StorageBuffer> pointLightBuffer;
		Asset<StorageBuffer> spotLightBuffer;
	};
}