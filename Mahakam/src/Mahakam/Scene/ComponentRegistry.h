#pragma once

#include "Entity.h"

#include <yaml-cpp/yaml.h>

namespace Mahakam
{
	class ComponentRegistry
	{
	public:
		struct ComponentInterface
		{
			bool (*HasComponent)(Entity) = nullptr;
			void (*AddComponent)(Entity) = nullptr;
			void (*RemoveComponent)(Entity) = nullptr;

			void (*OnInspector)(Entity) = nullptr;

			bool (*Serialize)(YAML::Emitter&, Entity) = nullptr;
			bool (*Deserialize)(YAML::Node&, Entity) = nullptr;

			template<typename T>
			void SetComponent()
			{
				HasComponent = [](Entity entity) { return entity.HasComponent<T>(); };
				AddComponent = [](Entity entity) { entity.AddComponent<T>(); };
				RemoveComponent = [](Entity entity) { entity.RemoveComponent<T>(); };
			}
		};

	private:
		using ComponentMap = UnorderedMap<std::string, ComponentInterface>;

		static ComponentMap componentInterfaces;

	public:
		MH_DECLARE_FUNC(RegisterComponent, void, const std::string& name, ComponentInterface componentInterface);
		MH_DECLARE_FUNC(DeregisterComponent, void, const std::string& name);

		MH_DECLARE_FUNC(GetComponents, const ComponentMap&);
	};
}