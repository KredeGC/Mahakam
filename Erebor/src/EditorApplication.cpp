#include "ebpch.h"
#include "EditorApplication.h"
#include <Mahakam/Core/EntryPoint.h>

#include <imgui/imgui.h>

namespace Mahakam::Editor
{
	EditorApplication::EditorApplication() : Application({ "Erebor " + FileUtility::GetWorkingDirectory().string(), "internal/icons/icon-64.png" })
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