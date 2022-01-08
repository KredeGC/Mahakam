#pragma once

#include "Mahakam/Core/Timestep.h"

#include <entt/entt.hpp>

namespace Mahakam
{
	class Entity;

	class Scene
	{
	private:
		friend class Entity;
		friend class SceneHierarchyPanel;

		entt::registry registry;

	public:
		Scene();
		~Scene();

		void onUpdate(Timestep ts);

		void onViewportResize(uint32_t width, uint32_t height);

		Entity createEntity(const std::string& name = "Entity");

		static Ref<Scene> createScene();
	};
}