#pragma once

#include "Mahakam/Core/Timestep.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Components/CameraComponent.h"

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

		Asset<Material> skyboxMaterial;
		Asset<TextureCube> skyboxTexture;
		Asset<TextureCube> skyboxIrradiance;
		Asset<TextureCube> skyboxSpecular;

		entt::registry registry;

		float viewportRatio = 1.0f;

	public:
		Scene();
		Scene(const std::string& filepath);
		~Scene();

		void OnUpdate(Timestep ts, bool editor = false);
		void OnRender(Camera& camera, const glm::mat4& cameraTransform);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		template<typename Fn>
		void ForEachEntity(Fn func)
		{
			return registry.each(func);
		}

		template<typename ... Args, typename Fn>
		void ForEach(Fn func)
		{
			return registry.view<Args...>().each(func);
		}

		void SetSkyboxMaterial(Asset<Material> material) { skyboxMaterial = material; }
		void SetSkyboxIrradiance(Asset<TextureCube> irradiance) { skyboxIrradiance = irradiance; }
		void SetSkyboxSpecular(Asset<TextureCube> specular) { skyboxSpecular = specular; }

		Asset<Material> GetSkyboxMaterial() const { return skyboxMaterial; }
		Asset<TextureCube> GetSkyboxIrradiance() const { return skyboxIrradiance; }
		Asset<TextureCube> GetSkyboxSpecular() const { return skyboxSpecular; }

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

	template<>
	inline void Scene::OnComponentAdded<CameraComponent>(const Entity& entity, CameraComponent& component)
	{
		component.GetCamera().SetRatio(viewportRatio);
	}
}