#include "mhpch.h"
#include "EditorWindowRegistry.h"

#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam::Editor
{
	EditorWindowRegistry* EditorWindowRegistry::s_Instance;

	EditorWindowRegistry* EditorWindowRegistry::GetInstance()
	{
		MH_OVERRIDE_FUNC(EditorWindowRegistryGetInstance);

		return s_Instance;
	}

	void EditorWindowRegistry::RegisterWindowImpl(EditorWindowProps props)
	{
		windowProps[props.Name] = props;
	}

	void EditorWindowRegistry::DeregisterWindowImpl(const std::string& name)
	{
		windowProps.erase(name);
	}

	void EditorWindowRegistry::OpenWindowImpl(const std::string& name)
	{
		auto iter = windowProps.find(name);
		if (iter != windowProps.end())
		{
			EditorWindow* window = iter->second.OpenWindow();
			windows.emplace(window);
			if (iter->second.Unique)
				iter->second.Instance = window;
		}
	}

	void EditorWindowRegistry::CloseWindowImpl(EditorWindow* window)
	{
		windows.erase(window);
	}
}