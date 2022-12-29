#pragma once

#include "Entity.h"
#include "SceneSerializer.h"

#include <ryml/rapidyaml-0.4.1.hpp>

namespace Mahakam
{
	class ComponentRegistry
	{
	public:
		struct ComponentInterface
		{
			const char* Icon = nullptr;

			bool (*HasComponent)(Entity) = nullptr;
			void (*AddComponent)(Entity) = nullptr;
			void (*CopyComponent)(Entity, Entity) = nullptr;
			void (*RemoveComponent)(Entity) = nullptr;

			bool (*Serialize)(ryml::NodeRef&, Entity) = nullptr;
			bool (*Deserialize)(ryml::NodeRef&, SceneSerializer::EntityMap&, Entity) = nullptr;

			template<typename T>
			void SetComponent()
			{
				HasComponent = [](Entity entity) { return entity.HasComponent<T>(); };
				AddComponent = [](Entity entity) { entity.AddComponent<T>(); };
				CopyComponent = [](Entity src, Entity dst) { dst.AddComponent<T>(src.GetComponent<T>()); };
				RemoveComponent = [](Entity entity) { entity.RemoveComponent<T>(); };
			}
		};

	private:
		using ComponentMap = UnorderedMap<std::string, ComponentInterface>;

		static ComponentMap s_ComponentInterfaces;

	public:
		MH_DECLARE_FUNC(RegisterComponent, void, const std::string& name, const ComponentInterface& componentInterface);
		MH_DECLARE_FUNC(DeregisterComponent, void, const std::string& name);
		MH_DECLARE_FUNC(RegisterDefaultComponents, void);
		MH_DECLARE_FUNC(DeregisterDefaultComponents, void);

		MH_DECLARE_FUNC(GetComponents, const ComponentMap&);
	};
}