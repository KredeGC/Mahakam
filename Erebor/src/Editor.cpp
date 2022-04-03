#include "ebpch.h"
#include "Editor.h"
#include <Mahakam/Core/EntryPoint.h>

#include <imgui.h>

namespace Mahakam
{
	Editor::Editor() : Application({ "Erebor", "assets/textures/internal/icon.png" })
	{
		m_EditorLayer = new EditorLayer();

		PushOverlay(m_EditorLayer);

		//getWindow().setVSync(true);
	}

	Editor::~Editor()
	{
		PopOverlay(m_EditorLayer);
	}

	extern Application* CreateApplication()
	{
		return new Editor();
	}
}