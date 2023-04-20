#pragma once

#include "Mahakam/Core/Types.h"

#include "Mahakam/Scene/Entity.h"

#include <ryml/rapidyaml-0.4.1.hpp>

#include <filesystem>
#include <string>

namespace Mahakam
{
	class Scene;

	class SceneSerializer
	{
	public:
		using EntityMap = UnorderedMap<uint32_t, entt::entity>;

	private:
		Ref<Scene> m_Scene;
		EntityMap m_Entities;

	public:
		SceneSerializer(Ref<Scene> scene);

		std::string SerializeToString();
		bool DeserializeFromString(const std::string& src);

		void SerializeToPath(const std::filesystem::path& filepath);
		bool DeserializeFromPath(const std::filesystem::path& filepath);

		void SerializeToTree(ryml::Tree& tree);
		bool DeserializeFromTree(ryml::Tree& tree);

	private:
		void SerializeEntity(ryml::NodeRef& entities, Entity entity);
		void DeserializeEntity(ryml::NodeRef& node, Entity entity);
	};
}