#include "mhpch.h"
#include "EditorWindowRegistry.h"

#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam::Editor
{
	std::unordered_map<std::string, EditorWindowRegistry::EditorWindowProps> EditorWindowRegistry::windowProps;
	std::unordered_set<EditorWindow*> EditorWindowRegistry::windows;

	//void EditorWindowRegistry::RegisterWindow(EditorWindowProps props)
	MH_DEFINE_FUNC(EditorWindowRegistry::RegisterWindow, void, EditorWindowProps props)
	{
		windowProps[props.Name] = props;
	};

	//void EditorWindowRegistry::DeregisterWindow(const std::string& name)
	MH_DEFINE_FUNC(EditorWindowRegistry::DeregisterWindow, void, const std::string& name)
	{
		windowProps.erase(name);
	};

	//void EditorWindowRegistry::OpenWindow(const std::string& name)
	MH_DEFINE_FUNC(EditorWindowRegistry::OpenWindow, EditorWindow*, const std::string& name)
	{
		auto iter = windowProps.find(name);
		if (iter != windowProps.end())
		{
			EditorWindow* window = iter->second.OpenWindow();
			windows.emplace(window);
			if (iter->second.Unique)
				iter->second.Instance = window;

			return window;
		}

		return nullptr;
	};

	//void EditorWindowRegistry::CloseWindow(EditorWindow* window)
	MH_DEFINE_FUNC(EditorWindowRegistry::CloseWindow, void, EditorWindow* window)
	{
		windows.erase(window);
	};

	//const EditorWindowRegistry::WindowPropsMap& EditorWindowRegistry::GetWindowProps()
	MH_DEFINE_FUNC(EditorWindowRegistry::GetWindowProps, const EditorWindowRegistry::WindowPropsMap&)
	{
		return windowProps;
	};

	//std::unordered_set<EditorWindow*>& EditorWindowRegistry::GetWindows()
	MH_DEFINE_FUNC(EditorWindowRegistry::GetWindows, std::unordered_set<EditorWindow*>&)
	{
		return windows;
	};
}