#include "ebpch.h"
#include "EditorApplication.h"
#include <Mahakam/Core/EntryPoint.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	EditorApplication::EditorApplication() : Application({ "Erebor", "internal/icons/icon-editor.png" })
	{
		// Create editor layer
		m_EditorLayer = new EditorLayer();

		PushOverlay(m_EditorLayer);

		//GetWindow().SetVSync(true);
	}

	EditorApplication::~EditorApplication()
	{

	}
}

extern Mahakam::Application* Mahakam::CreateApplication()
{
	return new Mahakam::Editor::EditorApplication();
}