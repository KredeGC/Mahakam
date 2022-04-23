#include "mhpch.h"
#include "ComponentRegistry.h"

#include "Entity.h"

#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam
{
	ComponentRegistry* ComponentRegistry::s_Instance = nullptr;

	ComponentRegistry* ComponentRegistry::GetInstance()
	{
		MH_OVERRIDE_FUNC(ComponentRegistryGetInstance);

		return s_Instance;
	}

	void ComponentRegistry::RegisterComponentImpl(const std::string& name, ComponentInterface componentInterface)
	{
		componentInterfaces[name] = componentInterface;
	}

	void ComponentRegistry::DeregisterComponentImpl(const std::string& name)
	{
		componentInterfaces.erase(name);
	}
}