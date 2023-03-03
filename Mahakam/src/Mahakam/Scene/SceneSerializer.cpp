#include "Mahakam/mhpch.h"
#include "SceneSerializer.h"

#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Shader.h"

#include "Components/RelationshipComponent.h"
#include "Components/TagComponent.h"

#include "ComponentRegistry.h"
#include "Entity.h"

#include "Mahakam/Serialization/YAMLGuard.h"
#include "Mahakam/Serialization/YAMLSerialization.h"

#include <ryml/rapidyaml-0.4.1.hpp>

namespace Mahakam
{
	SceneSerializer::SceneSerializer(Ref<Scene> scene)
		: m_Scene(scene) {}

	std::string SceneSerializer::Serialize()
	{
		ryml::Tree tree;

		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		root["Scene"] << "Untitled";

		// Skybox
		ryml::NodeRef skybox = root["Skybox"];
		skybox |= ryml::MAP;
		skybox["Material"] << m_Scene->GetSkyboxMaterial().GetID();
		skybox["Irradiance"] << m_Scene->GetSkyboxIrradiance().GetID();
		skybox["Specular"] << m_Scene->GetSkyboxSpecular().GetID();

		// Entities
		ryml::NodeRef entities = root["Entities"];
		entities |= ryml::SEQ;

		m_Scene->ForEachEntity([&](Entity entity)
		{
			if (!entity)
				return;

			// Serialize while sorting
			if (entity.HasComponent<RelationshipComponent>())
			{
				auto& relation = entity.GetComponent<RelationshipComponent>();

				if (relation.Parent == entt::null)
					SerializeEntity(entities, entity);
			}
			else
			{
				SerializeEntity(entities, entity);
			}
		});

		std::stringstream ss;
		ss << tree;
		return ss.str();
	}

	bool SceneSerializer::Deserialize(const std::string& src)
	{
		try
		{
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(src));

			return DeserializeFromTree(tree);
		}
		catch (std::runtime_error const& e)
		{
			MH_WARN("Scene was unable to load from YAML source: {0}", e.what());
		}

		return false;
	}

	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		std::string yamlString = Serialize();

		std::ofstream filestream(filepath);
		filestream << yamlString;
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		TrivialVector<char> buffer;

		if (!FileUtility::ReadFile(filepath, buffer))
			return false;

		try
		{
			ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(buffer.data(), buffer.size()));

			return DeserializeFromTree(tree);
		}
		catch (std::runtime_error const& e)
		{
			MH_WARN("Scene was unable to load from YAML file {0}: {1}", filepath.string(), e.what());
		}

		return false;
	}

	bool SceneSerializer::DeserializeFromTree(ryml::Tree& tree)
	{
		m_Entities.clear();

		ryml::NodeRef root = tree.rootref();

		if (!root.has_child("Scene"))
			return false;

		if (root.has_child("Skybox"))
		{
			ryml::NodeRef skyboxNode = root["Skybox"];

			Asset<Material> skyboxMaterial;
			uint64_t assetID = 0;

			if (skyboxNode.has_child("Material"))
			{
				ryml::NodeRef materialNode = skyboxNode["Material"];
				materialNode >> assetID;

				skyboxMaterial = Asset<Material>(assetID);
			}

			if (!skyboxMaterial)
			{
				Asset<Shader> skyboxShader = Asset<Shader>(Shader::Create("assets/shaders/Skybox.shader"));
				skyboxMaterial = Asset<Material>(Material::Create(skyboxShader));
			}
			m_Scene->SetSkyboxMaterial(skyboxMaterial);

			if (skyboxNode.has_child("Irradiance"))
			{
				ryml::NodeRef irradianceNode = skyboxNode["Irradiance"];
				irradianceNode >> assetID;

				Asset<TextureCube> irradianceTexture = Asset<TextureCube>(assetID);
				m_Scene->SetSkyboxIrradiance(irradianceTexture);
			}

			if (skyboxNode.has_child("Specular"))
			{
				ryml::NodeRef specularNode = skyboxNode["Specular"];
				specularNode >> assetID;

				Asset<TextureCube> specularTexture = Asset<TextureCube>(assetID);
				m_Scene->SetSkyboxSpecular(specularTexture);
			}
		}

		if (root.has_child("Entities"))
		{
			ryml::NodeRef entities = root["Entities"];

			m_Entities.reserve(entities.num_children());

			// Create entities and their relationships
			for (auto entity : entities)
			{
				uint32_t id = entt::null;
				if (entity.has_child("ID"))
					entity["ID"] >> id;

				uint32_t parent = entt::null;
				if (entity.has_child("Parent"))
					entity["Parent"] >> parent;

				std::string tag;
				if (entity.has_child("Tag"))
					entity["Tag"] >> tag;

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

				m_Entities[id] = deserializedEntity.GetEntt();
			}

			// Deserialize other components
			for (auto entity : entities)
			{
				uint32_t id = entt::null;
				if (entity.has_child("ID"))
					entity["ID"] >> id;

				Entity deserializedEntity{ m_Entities[id], m_Scene.get() };

				DeserializeEntity(entity, deserializedEntity);
			}
		}

		m_Scene->Sort();

		return true;
	}

	void SceneSerializer::SerializeEntity(ryml::NodeRef& entities, Entity entity)
	{
		ryml::NodeRef entityNode = entities.append_child({ ryml::MAP });

		if (entity.HasComponent<TagComponent>())
		{
			TagComponent& tag = entity.GetComponent<TagComponent>();

			entityNode["ID"] << entity.GetHandle();
			entityNode["Tag"] << tag.Tag;
		}

		if (entity.HasComponent<RelationshipComponent>())
		{
			RelationshipComponent& relation = entity.GetComponent<RelationshipComponent>();

			if (relation.Parent != entt::null)
				entityNode["Parent"] << uint32_t(relation.Parent);
		}

		for (auto& [name, componentInterface] : ComponentRegistry::GetComponents())
		{
			if (componentInterface.HasComponent(entity) && componentInterface.Serialize)
			{
				ryml::NodeRef component = entityNode[ryml::to_csubstr(name)];
				component |= ryml::MAP;

				componentInterface.Serialize(component, entity);
			}
		}

		if (entity.HasComponent<RelationshipComponent>())
		{
			RelationshipComponent& relation = entity.GetComponent<RelationshipComponent>();

			Entity current = entity.GetFirstChild();
			while (current)
			{
				SerializeEntity(entities, current);

				current = current.GetNext();
			}
		}
	}

	void SceneSerializer::DeserializeEntity(ryml::NodeRef& node, Entity entity)
	{
		for (auto& [name, componentInterface] : ComponentRegistry::GetComponents())
		{
			ryml::csubstr key = ryml::to_csubstr(name);
			if (node.has_child(key) && componentInterface.Deserialize)
			{
				ryml::NodeRef component = node[key];
				componentInterface.Deserialize(component, m_Entities, entity);
			}
		}
	}
}