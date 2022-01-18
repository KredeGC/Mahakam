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
		Ref<Texture> skyboxTexture;
		Ref<Texture> skyboxIrradiance;
		Ref<Texture> skyboxSpecular;

		entt::registry registry;

		float viewportRatio;

		template<typename T>
		void onComponentAdded(Entity entity, T& component)
		{
			//static_assert(false);
		}

		template<>
		void onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
		{
			component.getCamera().setRatio(viewportRatio);
		}

	public:
		Scene(const std::string& filepath);
		Scene(const Ref<Texture>& irradianceMap, const Ref<Texture>& specularMap);
		~Scene();

		void onUpdate(Timestep ts);

		void onViewportResize(uint32_t width, uint32_t height);

		Entity createEntity(const std::string& name = "Entity");
		void destroyEntity(Entity entity);

		static Ref<Scene> createScene(const std::string& filepath);
		static Ref<Scene> createScene(const Ref<Texture>& irradianceMap, const Ref<Texture>& specularMap);
	};
}