#pragma once

#include "Mahakam/Core/Timestep.h"

#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Texture.h"

#include "Components/CameraComponent.h"

#include "Entity.h"

#include <entt/entt.hpp>

namespace Mahakam
{
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
		Scene(const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap);
		~Scene();

		void OnUpdate(Timestep ts);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		static Ref<Scene> CreateScene(const std::string& filepath);
		static Ref<Scene> CreateScene(const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap);

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component)
		{
			//static_assert(false);
		}

		template<>
		void OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
		{
			component.GetCamera().SetRatio(viewportRatio);
		}
	};
}