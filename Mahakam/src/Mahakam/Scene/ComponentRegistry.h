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
			bool (*HasComponent)(Entity);
			void (*AddComponent)(Entity);
			void (*RemoveComponent)(Entity);

			void (*OnInspector)(Entity);

			bool (*Serialize)(YAML::Emitter&, Entity);
			bool (*Deserialize)(YAML::Node&, Entity);
		};

	private:
		std::unordered_map<std::string, ComponentInterface> componentInterfaces;

		static ComponentRegistry* s_Instance;

	public:
		// TODO: Use a singleton and override it in SharedLibrary
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