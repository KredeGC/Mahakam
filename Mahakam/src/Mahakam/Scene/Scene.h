#pragma once

#include "Mahakam/Core/Timestep.h"

#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Texture.h"

#include "Components/CameraComponent.h"

#include <entt/entt.hpp>

namespace Mahakam
{
	class Entity;

	class Scene
	{
	private:
		friend class Entity;
		friend class SceneHierarchyPanel;

		Ref<Material> skyboxMaterial;
		Ref<TextureCube> skyboxTexture;
		Ref<TextureCube> skyboxIrradiance;
		Ref<TextureCube> skyboxSpecular;

		entt::registry registry;

		float viewportRatio = 1.0f;

	public:
		Scene(const std::string& filepath);
		Scene(const Ref<TextureCube>& skybox, const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap);
		~Scene();

		void OnUpdate(Timestep ts, bool dontRender = false);
		void OnRender(Camera& camera, const glm::mat4& cameraTransform);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		template<typename ... Args, typename Fn>
		void ForEach(Fn func)
		{
			return registry.view<Args...>().each(func);
		}

		static Ref<Scene> CreateScene(const std::string& filepath);
		static Ref<Scene> CreateScene(const Ref<TextureCube>& skybox, const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap);

	private:
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