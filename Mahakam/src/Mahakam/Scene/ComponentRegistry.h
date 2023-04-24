#pragma once

#include "Entity.h"

#include <ryml/rapidyaml-0.4.1.hpp>

#include <type_traits>

namespace Mahakam
{
	class ComponentRegistry
	{
	public:
		struct ComponentInterface
		{
#ifndef MH_STANDALONE
			const char* Icon = nullptr;
			void (*OnPropertyDraw)(Entity) = nullptr;
#endif

			bool (*HasComponent)(Entity) = nullptr;
			void (*AddComponent)(Entity) = nullptr;
			void (*CopyComponent)(Entity, Entity) = nullptr;
			bool (*RemoveComponent)(Entity) = nullptr;

			bool (*Serialize)(ryml::NodeRef&, Entity) = nullptr;
			bool (*Deserialize)(ryml::NodeRef&, UnorderedMap<uint32_t, entt::entity>&, Entity) = nullptr;

			template<typename T>
			void SetComponent()
			{
				static_assert(std::is_constructible_v<T, const T&>, "Component must be copy constructible");

				AddComponent = [](Entity entity) { entity.AddComponent<T>(); };
				HasComponent = [](Entity entity) { return entity.HasComponent<T>(); };
				RemoveComponent = [](Entity entity) { return entity.RemoveComponent<T>(); };

				if constexpr (!std::is_empty_v<T>)
					CopyComponent = [](Entity src, Entity dst) { dst.AddComponent<T>(src.GetComponent<T>()); };
				else
					CopyComponent = [](Entity src, Entity dst) { dst.AddComponent<T>(); };
			}

			inline void SetEditor(const char* icon = nullptr, void (*onPropertyDraw)(Entity) = nullptr)
			{
#ifndef MH_STANDALONE
				Icon = icon;
				OnPropertyDraw = onPropertyDraw;
#endif
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