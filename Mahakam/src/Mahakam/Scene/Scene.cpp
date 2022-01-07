#include "mhpch.h"
#include "Scene.h"

#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"

#include "Mahakam/Renderer/Renderer.h"

#include "Entity.h"

namespace Mahakam
{
	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}

	void Scene::onUpdate(Timestep ts)
	{
		auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto& entity : group)
		{
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

			//Renderer::submit(transform, mesh);
		}
	}

	Entity Scene::createEntity()
	{
		Entity entity(registry.create(), this);
		entity.addComponent<TransformComponent>();
		return entity;
	}

	Ref<Scene> Scene::createScene()
	{
		return std::make_shared<Scene>();
	}
}