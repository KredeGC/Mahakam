#pragma once

#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Timestep.h"

#include "Mahakam/Asset/Asset.h"

#include "Mahakam/Physics/PhysicsEngine.h"

#include "Mahakam/Renderer/EnvironmentData.h"
#include "Mahakam/Renderer/Camera.h"

#include <entt/entt.hpp>

namespace Mahakam
{
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
		bool m_Unsorted = true;

		PhysicsContext* m_PhysicsContext;

	public:
		Scene(PhysicsContext* physics);
		Scene(const std::string& filepath, PhysicsContext* physics);
		~Scene();

		void OnUpdate(Timestep ts, bool editor = false);
		void OnRender(Camera& camera, const glm::mat4& cameraTransform);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		template<typename Fn>
		void ForEachEntity(Fn func);

		template<typename Fn>
		void ForEachEntityReverse(Fn func);

		template<typename ... Args, typename Fn>
		void ForEach(Fn func);

		template<typename ... Args>
		void DestroyAllEntities();

		void SetSkyboxMaterial(Asset<Material> material) { m_Environment.SkyboxMaterial = std::move(material); }
		void SetSkyboxIrradiance(Asset<TextureCube> irradiance) { m_Environment.IrradianceMap = std::move(irradiance); }
		void SetSkyboxSpecular(Asset<TextureCube> specular) { m_Environment.SpecularMap = std::move(specular); }

		Asset<Material> GetSkyboxMaterial() const { return m_Environment.SkyboxMaterial; }
		Asset<TextureCube> GetSkyboxIrradiance() const { return m_Environment.IrradianceMap; }
		Asset<TextureCube> GetSkyboxSpecular() const { return m_Environment.SpecularMap; }

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

#include "Entity.h"
#include "Components/CameraComponent.h"

namespace Mahakam
{
	template<typename Fn>
	void Scene::ForEachEntity(Fn func)
	{
		const entt::entity* data = m_Registry.data();
		for (size_t i = 0; i < m_Registry.size(); i++)
		{
			const entt::entity& handle = data[i];
			if (m_Registry.valid(handle))
			{
				Entity entity{ handle, this };
				func(entity);
			}
		}
	}

	template<typename Fn>
	void Scene::ForEachEntityReverse(Fn func)
	{
		return m_Registry.each(func);
	}

	template<typename ... Args, typename Fn>
	void Scene::ForEach(Fn func)
	{
		return m_Registry.view<Args...>().each(func);
	}

	template<typename ... Args>
	void Scene::DestroyAllEntities()
	{
		auto view = m_Registry.view<Args...>();
		m_Registry.destroy(view.begin(), view.end());
	}

	template<>
	inline void Scene::OnComponentAdded<CameraComponent>(const Entity& entity, CameraComponent& component)
	{
		component.GetCamera().SetRatio(m_ViewportRatio);
	}
}