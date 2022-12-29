#pragma once

#include "Mahakam/Core/Allocator.h"
#include "Mahakam/Core/Core.h"

#include "Mahakam/Core/SharedLibrary.h"

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
				OpenWindow = []() { return (EditorWindow*)Allocator::New<T>(); };
			}
		};

		using WindowPropsMap = UnorderedMap<std::string, EditorWindowProps>;

	private:
		static WindowPropsMap windowProps;
		static UnorderedSet<EditorWindow*> windows;

	public:
		MH_DECLARE_FUNC(RegisterWindow, void, EditorWindowProps props); // TODO: Use const &
		MH_DECLARE_FUNC(DeregisterWindow, void, const std::string& name);

		MH_DECLARE_FUNC(OpenWindow, EditorWindow*, const std::string& name);
		MH_DECLARE_FUNC(CloseWindow, UnorderedSet<EditorWindow*>::iterator, EditorWindow* window);

		MH_DECLARE_FUNC(GetWindowProps, const WindowPropsMap&);
		MH_DECLARE_FUNC(GetWindows, UnorderedSet<EditorWindow*>&);

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