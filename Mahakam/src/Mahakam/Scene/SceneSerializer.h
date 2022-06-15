#pragma once

#include "Scene.h"

#include <yaml-cpp/yaml.h>

namespace Mahakam
{
	class SceneSerializer
	{
	private:
		Ref<Scene> m_Scene;

	public:
		SceneSerializer(Ref<Scene> scene);

		void Serialize(const std::string& filepath);
		bool Deserialize(const std::string& filepath);

	private:
		void SerializeEntity(YAML::Emitter& emitter, Entity entity);
		void DeserializeEntity(YAML::Node& node, Entity entity);
	};
}