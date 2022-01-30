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

		float viewportRatio;

	public:
		Scene(const std::string& filepath);
		Scene(const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap);
		~Scene();

		void onUpdate(Timestep ts);

		void onViewportResize(uint32_t width, uint32_t height);

		Entity createEntity(const std::string& name = "Entity");
		void destroyEntity(Entity entity);

		static Ref<Scene> createScene(const std::string& filepath);
		static Ref<Scene> createScene(const Ref<TextureCube>& irradianceMap, const Ref<TextureCube>& specularMap);

	private:
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
	};
}