#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Core/SharedLibrary.h"

#include <unordered_set>

namespace Mahakam::Editor
{
	class EditorWindow;

	class EditorWindowRegistry
	{
	public:
		struct EditorWindowProps
		{
			std::string Name;

			// TODO: Different ways of opening manually, eg. in the "View" tab
			bool Viewable = true;
			bool Unique = true; // Whether only one instance should be allowed

			EditorWindow* (*OpenWindow)() = nullptr;

			EditorWindow* Instance = nullptr;

			template<typename T>
			void SetWindow()
			{
				OpenWindow = []() { return (EditorWindow*)new T(); };
			}
		};

	private:
		using WindowPropsMap = UnorderedMap<std::string, EditorWindowProps>;

		static WindowPropsMap windowProps;
		static std::unordered_set<EditorWindow*> windows;

	public:
		MH_DECLARE_FUNC(RegisterWindow, void, EditorWindowProps props); // TODO: Use const &
		MH_DECLARE_FUNC(DeregisterWindow, void, const std::string& name);

		MH_DECLARE_FUNC(OpenWindow, EditorWindow*, const std::string& name);
		MH_DECLARE_FUNC(CloseWindow, std::unordered_set<EditorWindow*>::iterator, EditorWindow* window);

		MH_DECLARE_FUNC(GetWindowProps, const WindowPropsMap&);
		MH_DECLARE_FUNC(GetWindows, std::unordered_set<EditorWindow*>&);

		template<typename T>
		static void RegisterWindowClass(const std::string& name)
		{
			EditorWindowProps panelProps;
			panelProps.Name = name;
			panelProps.SetWindow<T>();

			RegisterWindow(panelProps);
		}
	};
}