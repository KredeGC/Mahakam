#include "mhpch.h"
#include "Scene.h"

#include "Components/CameraComponent.h"
#include "Components/MeshComponent.h"
#include "Components/NativeScriptComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"

#include "Mahakam/Renderer/Renderer.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace Mahakam
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& q)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << q.x << q.y << q.z << q.w << YAML::EndSeq;
		return out;
	}

	static void serializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << "123679821";

		if (entity.hasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			auto& tag = entity.getComponent<TagComponent>().tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}

		if (entity.hasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = entity.getComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.getPosition();
			out << YAML::Key << "Rotation" << YAML::Value << tc.getRotation();
			out << YAML::Key << "Scale" << YAML::Value << tc.getScale();

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.hasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.getComponent<CameraComponent>();
			auto& camera = cameraComponent.getCamera();

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.getProjectionType();
			out << YAML::Key << "FOV" << YAML::Value << camera.getFov();
			out << YAML::Key << "Near" << YAML::Value << camera.getNearPlane();
			out << YAML::Key << "Far" << YAML::Value << camera.getFarPlane();
			out << YAML::Key << "Size" << YAML::Value << camera.getSize();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.hasFixedAspectRatio();

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.hasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap; // MeshComponent

			auto& meshComponent = entity.getComponent<MeshComponent>();
			//out << YAML::Key << "Color" << YAML::Value << meshComponent;

			out << YAML::EndMap; // MeshComponent
		}

		out << YAML::EndMap;
	}

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

			Renderer::endScene();
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

	bool Scene::serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		registry.each([&](auto entityID)
		{
			Entity entity = { entityID, this };
			if (!entity)
				return;

			serializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	bool Scene::deserialize(const std::string& filepath)
	{
		/*std::ifstream stream(filepath);
		std::stringstream stringStream;

		stringStream << stream.rdbuf();

		YAML::Node data = YAML::Load(stringStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		MH_CORE_TRACE("Deserializing scene {0}", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto& entityNode : entities)
			{
				if (entityNode["CameraComponent"])
				{
					uint64_t uuid = entityNode["Entity"].as<uint64_t>();

					std::string name;
					auto tagComponent = entityNode["TagComponent"];
					if (tagComponent)
						name = tagComponent["Tag"].as<std::string>();

					MH_CORE_TRACE("Deserialized Entity ({0}), {1}", uuid, name);

					Entity entity = createEntity(name);

					auto transformComponent = entityNode["TransformComponent"];
					if (transformComponent)
					{
						auto& transform = entity.getComponent<TransformComponent>();
						transform.setPosition(transformComponent["Translation"].as<glm::vec3>());
						transform.setRotation(transformComponent["Rotation"].as<glm::quat>());
						transform.setScale(transformComponent["Scale"].as<glm::vec3>());
					}


				}
			}
		}*/

		return true;
	}

	bool Scene::serializeRuntime(const std::string& filepath)
	{
		MH_CORE_BREAK("Not implemented!");
		return false;
	}

	bool Scene::deserializeRuntime(const std::string& filepath)
	{
		MH_CORE_BREAK("Not implemented!");
		return false;
	}
}