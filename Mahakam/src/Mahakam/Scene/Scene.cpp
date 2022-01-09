#include "mhpch.h"
#include "Scene.h"

#include "Components/CameraComponent.h"
#include "Components/MeshComponent.h"
#include "Components/NativeScriptComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"

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
		// Update scripts
		registry.view<NativeScriptComponent>().each([=](auto entity, auto& scriptComponent)
		{
			// TODO: onScenePlay
			for (auto& runtime : scriptComponent.scripts)
			{
				if (!runtime.script)
				{
					runtime.script = runtime.instantiateScript();
					runtime.script->entity = Entity{ entity, this };
					runtime.script->onCreate();
				}

				runtime.script->onUpdate(ts);
			}
		});



		uint32_t drawCalls;
		uint32_t vertexCount;
		uint32_t triCount;

		Ref<Light> mainLight = std::make_shared<Light>(glm::vec3(1.0f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));

		// Get the rendering camera
		CameraComponent* mainCamera = nullptr;
		TransformComponent* mainTransform = nullptr;
		auto cameras = registry.view<TransformComponent, CameraComponent>();
		for (auto& entity : cameras)
		{
			auto [transform, camera] = cameras.get<TransformComponent, CameraComponent>(entity);

			// Recalculate all projection matrices, if they've changed
			camera.getCamera().recalculateProjectionMatrix();

			mainCamera = &camera;
			mainTransform = &transform;
		}

		// Render each entity with a mesh
		if (mainCamera && mainTransform)
		{
			Renderer::beginScene(*mainCamera, *mainTransform, mainLight);

			auto meshes = registry.view<TransformComponent, MeshComponent>();
			for (auto& entity : meshes)
			{
				auto [transform, meshComponent] = meshes.get<TransformComponent, MeshComponent>(entity);

				Ref<Mesh> mesh = meshComponent.getMesh();
				Ref<Material> material = meshComponent.getMaterial();

				if (mesh && material)
					Renderer::submit(transform, mesh, material);
			}

			Renderer::endScene(&drawCalls, &vertexCount, &triCount);
		}
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		viewportRatio = (float)width / (float)height;

		auto cameras = registry.view<CameraComponent>();
		for (auto& entity : cameras)
		{
			CameraComponent& cameraComponent = cameras.get<CameraComponent>(entity);

			if (!cameraComponent.hasFixedAspectRatio())
			{
				cameraComponent.getCamera().setRatio(viewportRatio);
			}
		}
	}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity(registry.create(), this);
		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::destroyEntity(Entity entity)
	{
		registry.destroy(entity);
	}

	Ref<Scene> Scene::createScene()
	{
		return std::make_shared<Scene>();
	}
}