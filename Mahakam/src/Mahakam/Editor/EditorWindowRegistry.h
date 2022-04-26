#pragma once

#include "Mahakam/Core/Core.h"

#include "EditorWindow.h"

#include <unordered_set>

namespace Mahakam::Editor
{
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
		std::unordered_map<std::string, EditorWindowProps> windowProps;

		std::unordered_set<EditorWindow*> windows;

		static EditorWindowRegistry* s_Instance;

	public:
		static EditorWindowRegistry* GetInstance();

		inline static void Init() { s_Instance = new EditorWindowRegistry(); }
		inline static void Shutdown() { delete s_Instance; }

		inline static void RegisterWindow(EditorWindowProps props) { GetInstance()->RegisterWindowImpl(props); }
		inline static void DeregisterWindow(const std::string& name) { GetInstance()->DeregisterWindowImpl(name); }

		inline static void OpenWindow(const std::string& name) { GetInstance()->OpenWindowImpl(name); }
		inline static void CloseWindow(EditorWindow* window) { GetInstance()->CloseWindowImpl(window); }

		inline static const std::unordered_map<std::string, EditorWindowProps>& GetWindowProps() { return GetInstance()->GetWindowPropsImpl(); }
		inline static std::unordered_set<EditorWindow*>& GetWindows() { return GetInstance()->GetWindowsImpl(); }

	private:
		void RegisterWindowImpl(EditorWindowProps props);
		void DeregisterWindowImpl(const std::string& name);

		void OpenWindowImpl(const std::string& name);
		void CloseWindowImpl(EditorWindow* window);

		const std::unordered_map<std::string, EditorWindowProps>& GetWindowPropsImpl() const { return windowProps; }
		std::unordered_set<EditorWindow*>& GetWindowsImpl() { return windows; }
	};
}