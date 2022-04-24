#pragma once

#include "Scene.h"

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
		std::unordered_map<std::string, ComponentInterface> componentInterfaces;

		static ComponentRegistry* s_Instance;

	public:
		static ComponentRegistry* GetInstance();

		static void Init() { s_Instance = new ComponentRegistry(); }
		static void Shutdown() { delete s_Instance; }

		inline static void RegisterComponent(const std::string& name, ComponentInterface componentInterface) { GetInstance()->RegisterComponentImpl(name, componentInterface); }
		inline static void DeregisterComponent(const std::string& name) { GetInstance()->DeregisterComponentImpl(name); }

		inline static const std::unordered_map<std::string, ComponentInterface>& GetComponents() { return GetInstance()->GetComponentsImpl(); }

	private:
		void RegisterComponentImpl(const std::string& name, ComponentInterface componentInterface);
		void DeregisterComponentImpl(const std::string& name);

		const std::unordered_map<std::string, ComponentInterface>& GetComponentsImpl() const { return componentInterfaces; }
	};
}