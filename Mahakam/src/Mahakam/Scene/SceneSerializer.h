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

		std::string Serialize();
		bool Deserialize(const std::string& src);

		void Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		bool DeserializeFromTree(ryml::Tree& tree);

		void SerializeEntity(ryml::NodeRef& entities, Entity entity);
		void DeserializeEntity(ryml::NodeRef& node, Entity entity);
	};
}