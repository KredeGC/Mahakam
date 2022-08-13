#include "Mahakam/mhpch.h"
#include "SceneSerializer.h"

#include "Mahakam/Math/Math.h"

#include "Mahakam/Renderer/Material.h"

#include "Components/TagComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/TransformComponent.h"

#include "ComponentRegistry.h"
#include "Entity.h"

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
		emitter << YAML::Value << "Untitled";

		emitter << YAML::Key << "Skybox";
		emitter << YAML::Value << YAML::BeginMap;

		emitter << YAML::Key << "Material";
		emitter << YAML::Value << m_Scene->GetSkyboxMaterial().GetID();
		emitter << YAML::Key << "Irradiance";
		emitter << YAML::Value << m_Scene->GetSkyboxIrradiance().GetID();
		emitter << YAML::Key << "Specular";
		emitter << YAML::Value << m_Scene->GetSkyboxSpecular().GetID();

		emitter << YAML::Value << YAML::EndMap;

		emitter << YAML::Key << "Entities";
		emitter << YAML::Value << YAML::BeginSeq;
		m_Scene->ForEachEntity([&](Entity entity)
		{
			if (!entity)
				return;

			// Serialize while sorting
			if (entity.HasComponent<RelationshipComponent>())
			{
				auto& relation = entity.GetComponent<RelationshipComponent>();

				if (!relation.Parent)
					SerializeEntity(emitter, entity);
			}
			else
			{
				SerializeEntity(emitter, entity);
			}
		});
		emitter << YAML::EndSeq;
		emitter << YAML::EndMap;

		std::ofstream filestream(filepath);
		filestream << emitter.c_str();
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		m_Entities.clear();

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

		YAML::Node skyboxNode = data["Skybox"];
		if (skyboxNode)
		{
			Asset<Material> skyboxMaterial;
			YAML::Node materialNode = skyboxNode["Material"];
			if (materialNode)
				skyboxMaterial = Asset<Material>(materialNode.as<uint64_t>());
			if (!skyboxMaterial)
			{
				Asset<Shader> skyboxShader = Shader::Create("assets/shaders/Skybox.shader");
				skyboxMaterial = Material::Create(skyboxShader);
			}
			m_Scene->SetSkyboxMaterial(skyboxMaterial);

			YAML::Node irradianceNode = skyboxNode["Irradiance"];
			if (irradianceNode)
			{
				Asset<TextureCube> irradianceTexture = Asset<TextureCube>(irradianceNode.as<uint64_t>());
				m_Scene->SetSkyboxIrradiance(irradianceTexture);
			}

			YAML::Node specularNode = skyboxNode["Specular"];
			if (specularNode)
			{
				Asset<TextureCube> specularTexture = Asset<TextureCube>(specularNode.as<uint64_t>());
				m_Scene->SetSkyboxSpecular(specularTexture);
			}
		}

		auto entities = data["Entities"];
		if (entities)
		{
			m_Entities.reserve(entities.size());

			for (auto entity : entities)
			{
				uint32_t id = entt::null;
				auto idNode = entity["ID"];
				if (idNode)
					id = idNode.as<uint32_t>();

				uint32_t parent = entt::null;
				auto parentNode = entity["Parent"];
				if (parentNode)
					parent = parentNode.as<uint32_t>();

				std::string tag;
				auto tagNode = entity["Tag"];
				if (tagNode)
					tag = tagNode.as<std::string>();

				MH_CORE_TRACE(tag);

				Entity deserializedEntity = m_Scene->CreateEntity(tag);
				if (parent)
				{
					auto iter = m_Entities.find(parent);
					if (iter != m_Entities.end())
					{
						Entity parentEntity = { iter->second, m_Scene.get() };
						deserializedEntity.SetParent(parentEntity);
					}
				}

				m_Entities[id] = deserializedEntity;

				DeserializeEntity(entity, deserializedEntity);
			}
		}

		return true;
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& emitter, Entity entity)
	{
		// m_Entities[entity] = uint32_t(entity);

		emitter << YAML::BeginMap;

		if (entity.HasComponent<TagComponent>())
		{
			TagComponent& tag = entity.GetComponent<TagComponent>();

			emitter << YAML::Key << "ID";
			emitter << YAML::Value << uint32_t(entity);
			emitter << YAML::Key << "Tag";
			emitter << YAML::Value << tag.Tag;
		}

		if (entity.HasComponent<RelationshipComponent>())
		{
			RelationshipComponent& relation = entity.GetComponent<RelationshipComponent>();

			if (relation.Parent)
			{
				emitter << YAML::Key << "Parent";
				emitter << YAML::Value << uint32_t(relation.Parent);
			}
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

		if (entity.HasComponent<RelationshipComponent>())
		{
			RelationshipComponent& relation = entity.GetComponent<RelationshipComponent>();

			Entity current = relation.First;
			while (current)
			{
				SerializeEntity(emitter, current);

				current = current.GetComponent<RelationshipComponent>().Next;
			}
		}
	}

	void SceneSerializer::DeserializeEntity(YAML::Node& node, Entity entity)
	{
		for (auto& [name, componentInterface] : ComponentRegistry::GetComponents())
		{
			YAML::Node component = node[name];
			if (component && componentInterface.Deserialize)
				componentInterface.Deserialize(component, entity);
		}
	}
}