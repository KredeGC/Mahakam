#pragma once

#include "Scene.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>

namespace Mahakam
{
	class SceneSerializer
	{
	private:
		Ref<Scene> m_Scene;
		UnorderedMap<uint32_t, entt::entity> m_Entities;

	public:
		SceneSerializer(Ref<Scene> scene);

		std::string Serialize();
		bool Deserialize(const std::string& src);

		void Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		bool DeserializeFromNode(YAML::Node& node);

		void SerializeEntity(YAML::Emitter& emitter, Entity entity);
		void DeserializeEntity(YAML::Node& node, Entity entity);
	};
}