#pragma once

#include "Mahakam/Core/Timestep.h"

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
		Scene();
		~Scene();

		void onUpdate(Timestep ts);

		void onViewportResize(uint32_t width, uint32_t height);

		Entity createEntity(const std::string& name = "Entity");
		void destroyEntity(Entity entity);

		static Ref<Scene> createScene();
	};
}