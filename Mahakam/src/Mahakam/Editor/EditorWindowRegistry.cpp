#include "mhpch.h"
#include "EditorWindowRegistry.h"

namespace Mahakam::Editor
{
	EditorWindowRegistry* EditorWindowRegistry::s_Instance;

	EditorWindowRegistry* EditorWindowRegistry::GetInstance()
	{
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
			windows.emplace(iter->second.OpenWindow());
	}

	void EditorWindowRegistry::CloseWindowImpl(EditorWindow* window)
	{
		windows.erase(window);
	}
}