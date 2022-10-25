#pragma once

#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Timestep.h"

#include "Mahakam/Asset/Asset.h"

#include "Mahakam/Renderer/Camera.h"
#include "Mahakam/Renderer/RenderData.h"

#include <entt/entt.hpp>

namespace Mahakam
{
	class Entity;
	class Material;
	class TextureCube;

	class Scene
	{
	private:
		friend class Entity;
		friend class SceneSerializer;

		EnvironmentData m_Environment;

		entt::registry m_Registry;

		float m_ViewportRatio = 1.0f;

	public:
		Scene();
		Scene(const std::string& filepath);
		~Scene();

		void OnUpdate(Timestep ts, bool editor = false);
		void OnRender(Camera& camera, const glm::mat4& cameraTransform);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		void Sort();

		template<typename Fn>
		void ForEachEntity(Fn func);

		template<typename Fn>
		void ForEachEntityReverse(Fn func);

		template<typename ... Args, typename Fn>
		void ForEach(Fn func);

		template<typename ... Args>
		void DestroyAllEntities();

		void SetSkyboxMaterial(Asset<Material> material) { m_Environment.SkyboxMaterial = material; }
		void SetSkyboxIrradiance(Asset<TextureCube> irradiance) { m_Environment.IrradianceMap = irradiance; }
		void SetSkyboxSpecular(Asset<TextureCube> specular) { m_Environment.SpecularMap = specular; }

		Asset<Material> GetSkyboxMaterial() const { return m_Environment.SkyboxMaterial; }
		Asset<TextureCube> GetSkyboxIrradiance() const { return m_Environment.IrradianceMap; }
		Asset<TextureCube> GetSkyboxSpecular() const { return m_Environment.SpecularMap; }

		inline static Ref<Scene> Create() { return CreateEmpty(); }
		inline static Ref<Scene> Create(const std::string& filepath) { return CreateFilepath(filepath); }

	private:
		MH_DECLARE_FUNC(CreateEmpty, Ref<Scene>);
		MH_DECLARE_FUNC(CreateFilepath, Ref<Scene>, const std::string& filepath);

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