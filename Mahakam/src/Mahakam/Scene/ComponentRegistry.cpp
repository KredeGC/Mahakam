#include "mhpch.h"
#include "ComponentRegistry.h"

#include "Entity.h"

#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam
{
	ComponentRegistry::ComponentMap ComponentRegistry::componentInterfaces;

	//void ComponentRegistry::RegisterComponent(const std::string& name, ComponentInterface componentInterface)
	MH_DEFINE_FUNC(ComponentRegistry::RegisterComponent, void, const std::string& name, ComponentInterface componentInterface)
	{
		componentInterfaces[name] = componentInterface;
	};

	//void ComponentRegistry::DeregisterComponent(const std::string& name)
	MH_DEFINE_FUNC(ComponentRegistry::DeregisterComponent, void, const std::string& name)
	{
		componentInterfaces.erase(name);
	};

	//const ComponentMap& ComponentRegistry::GetComponents()
	MH_DEFINE_FUNC(ComponentRegistry::GetComponents, const ComponentRegistry::ComponentMap&)
	{
		return componentInterfaces;
	};
}