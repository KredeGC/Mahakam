#pragma once

#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Timestep.h"

#include "Mahakam/Asset/Asset.h"

#include "Mahakam/Physics/PhysicsEngine.h"

#include "Mahakam/Renderer/EnvironmentData.h"

#include <entt/entity/registry.hpp>

namespace Mahakam
{
	class Camera;
	class Entity;
	class Material;
	class PhysicsContext;
	class TextureCube;

	class Scene
	{
	private:
		friend class Entity;
		friend class SceneSerializer;

		EnvironmentData m_Environment;

		entt::registry m_Registry;

		float m_ViewportRatio = 1.0f;

		PhysicsContext* m_PhysicsContext;

	public:
		Scene(PhysicsContext* physics);
		Scene(const std::string& filepath, PhysicsContext* physics);
		Scene(const Scene&);
		Scene(Scene&&) noexcept = default;

		void OnUpdate(Timestep ts, bool editor = false);
		void OnRender(Camera& camera, const glm::mat4& cameraTransform);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		template<typename Fn>
		void ForEachEntity(Fn func);

		template<typename Fn>
		void ForEachEntityReverse(Fn func);

		template<typename... Args, typename Fn>
		void ForEach(Fn func)
		{
			return m_Registry.view<Args...>().each(func);
		}

		inline void SetSkyboxMaterial(Asset<Material> material) { m_Environment.SkyboxMaterial = std::move(material); }
		inline void SetSkyboxIrradiance(Asset<TextureCube> irradiance) { m_Environment.IrradianceMap = std::move(irradiance); }
		inline void SetSkyboxSpecular(Asset<TextureCube> specular) { m_Environment.SpecularMap = std::move(specular); }

		inline Asset<Material> GetSkyboxMaterial() const { return m_Environment.SkyboxMaterial; }
		inline Asset<TextureCube> GetSkyboxIrradiance() const { return m_Environment.IrradianceMap; }
		inline Asset<TextureCube> GetSkyboxSpecular() const { return m_Environment.SpecularMap; }

		inline static Ref<Scene> Create(PhysicsContext* physics = PhysicsEngine::GetContext()) { return CreateEmpty(physics); }
		inline static Ref<Scene> Create(const std::string& filepath, PhysicsContext* physics = PhysicsEngine::GetContext()) { return CreateFilepath(filepath, physics); }

	private:
		MH_DECLARE_FUNC(CreateEmpty, Ref<Scene>, PhysicsContext* physics);
		MH_DECLARE_FUNC(CreateFilepath, Ref<Scene>, const std::string& filepath, PhysicsContext* physics);

		template<typename T>
		void OnComponentAdded(const Entity& entity, T& component)
		{
			//static_assert(false);
		}
	};
}