#include "mhpch.h"
#include "SceneSerializer.h"
#include "Entity.h"

#include "Mahakam/Math/Math.h"
#include "ComponentRegistry.h"
#include "Components.h"

namespace Mahakam
{
	SceneSerializer::SceneSerializer(Ref<Scene> scene)
		: m_Scene(scene)
	{ }

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter emitter;
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Scene";
		emitter << YAML::Value << "Name";
		emitter << YAML::Key << "Entities";
		emitter << YAML::Value << YAML::BeginSeq;
		m_Scene->registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			SerializeEntity(emitter, entity);
		});
		emitter << YAML::EndSeq;
		emitter << YAML::EndMap;

		std::ofstream filestream(filepath);
		filestream << emitter.c_str();
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>(); // ENTITY UUID GOES HERE

				std::string name = entity["Tag"].as<std::string>();

				Entity deserializedEntity = m_Scene->CreateEntity(name);

				DeserializeEntity(entity, deserializedEntity);
			}
		}

		return true;
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& emitter, Entity entity)
	{
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Entity";
		emitter << YAML::Value << 1234; // ENTITY UUID GOES HERE

		if (entity.HasComponent<TagComponent>())
		{
			emitter << YAML::Key << "Tag";
			emitter << YAML::Value << entity.GetComponent<TagComponent>().tag;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			emitter << YAML::Key << "Transform";
			emitter << YAML::BeginMap;

			auto& transform = entity.GetComponent<TransformComponent>();
			emitter << YAML::Key << "Translation" << YAML::Value << transform.GetPosition();
			emitter << YAML::Key << "Rotation" << YAML::Value << transform.GetRotation();
			emitter << YAML::Key << "Scale" << YAML::Value << transform.GetScale();

			emitter << YAML::EndMap;
		}

		for (auto& [name, componentInterface] : ComponentRegistry::GetComponents())
		{
			if (componentInterface.HasComponent(entity) && componentInterface.Serialize)
			{
				emitter << YAML::Key << name;
				emitter << YAML::BeginMap;

				componentInterface.Serialize(emitter, entity);

				emitter << YAML::EndMap;
			}
		}

		emitter << YAML::EndMap;
	}

	void SceneSerializer::DeserializeEntity(YAML::Node& node, Entity entity)
	{
		YAML::Node transformComponent = node["Transform"];
		if (transformComponent)
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();
			transform.SetPosition(transformComponent["Translation"].as<glm::vec3>());
			transform.SetRotation(transformComponent["Rotation"].as<glm::quat>());
			transform.SetScale(transformComponent["Scale"].as<glm::vec3>());
		}

		for (auto& [name, componentInterface] : ComponentRegistry::GetComponents())
		{
			YAML::Node component = node[name];
			if (component && componentInterface.Deserialize)
				componentInterface.Deserialize(component, entity);
		}
	}
}