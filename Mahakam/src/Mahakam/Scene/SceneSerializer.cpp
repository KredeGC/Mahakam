#include "mhpch.h"
#include "SceneSerializer.h"

#include "Mahakam/Math/Math.h"

#include "Mahakam/Renderer/Material.h"

#include "Components/TagComponent.h"
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
			for (auto entity : entities)
			{
				uint64_t id = entity["ID"].as<uint64_t>();

				uint64_t parentID = entity["ParentID"].as<uint64_t>();

				std::string name = entity["Tag"].as<std::string>();

				Entity deserializedEntity = m_Scene->CreateEntity(id, parentID, name);

				DeserializeEntity(entity, deserializedEntity);
			}
		}

		return true;
	}

	void SceneSerializer::SerializeEntity(YAML::Emitter& emitter, Entity entity)
	{
		emitter << YAML::BeginMap;

		if (entity.HasComponent<TagComponent>())
		{
			TagComponent& tag = entity.GetComponent<TagComponent>();

			emitter << YAML::Key << "ID";
			emitter << YAML::Value << tag.ID;
			emitter << YAML::Key << "Parent";
			emitter << YAML::Value << tag.ParentID;
			emitter << YAML::Key << "Tag";
			emitter << YAML::Value << tag.Tag;
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
		for (auto& [name, componentInterface] : ComponentRegistry::GetComponents())
		{
			YAML::Node component = node[name];
			if (component && componentInterface.Deserialize)
				componentInterface.Deserialize(component, entity);
		}
	}
}