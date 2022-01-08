#pragma once

#include "Mahakam/Core/Timestep.h"

#include <entt/ent.hpp>

namespace Mahakam
{
	class Entity;

	class Scene
	{
	private:
		friend class Entity;

		entt::registry registry;

	public:
		Scene();
		~Scene();

		void onUpdate(Timestep ts);

		void onViewportResize(uint32_t width, uint32_t height);

		Entity createEntity();

		static Ref<Scene> createScene();
	};
}