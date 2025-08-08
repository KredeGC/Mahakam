#include "ebpch.h"
#include "EreborApplication.h"
#include <Mahakam/Core/EntryPoint.h>

#include <imgui/imgui.h>

namespace Mahakam::Editor
{
	EreborApplication::EreborApplication() : Application("Erebor", { "Erebor " + FileUtility::GetWorkingDirectory().string(), "internal/icons/icon-64.png", 600, 800 })
	{
		// Create editor layer
		m_ImporterLayer = new ImporterLayer();

		PushOverlay(m_ImporterLayer);

		//GetWindow().SetVSync(true);
	}

	EreborApplication::~EreborApplication()
	{

	}
}

extern Mahakam::Application* Mahakam::CreateApplication()
{
	return new Mahakam::Editor::EreborApplication();
}