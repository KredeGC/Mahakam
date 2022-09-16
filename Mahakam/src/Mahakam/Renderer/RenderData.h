#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

#include "EnvironmentData.h"
#include "ParticleSystem.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include <vector>

namespace Mahakam
{
	// Forward declare most things, don't want to pollute the header files too much
	class Camera;
	class SubMesh;
	class Shader;
	class Material;
	class UniformBuffer;
	class StorageBuffer;

	extern template class Asset<Material>;
	extern template class Asset<Shader>;
	extern template class Asset<SubMesh>;

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
		TrivialVector<uint64_t> renderQueue;

		UnorderedMap<Ref<Shader>, uint64_t> shaderRefLookup;
		UnorderedMap<Ref<Material>, uint64_t> materialRefLookup;
		UnorderedMap<Ref<SubMesh>, uint64_t> meshRefLookup;

		UnorderedMap<uint64_t, Ref<Shader>> shaderIDLookup;
		UnorderedMap<uint64_t, Ref<Material>> materialIDLookup;
		UnorderedMap<uint64_t, Ref<SubMesh>> meshIDLookup;
		UnorderedMap<uint64_t, glm::mat4> transformIDLookup;
		UnorderedMap<uint64_t, glm::mat4> boneIDLookup; // TODO: Implement this

		// Particle queue ID
		// 32 bits - Depth
		// 16 bits - ParticleSystem index
		// 16 bits - Transform index
		TrivialVector<uint64_t> particleQueue;

		UnorderedMap<uint64_t, ParticleSystem> particleIDLookup;

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