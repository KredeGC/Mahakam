#include "ebpch.h"
#include <Mahakam.h>
#include <Mahakam/Core/EntryPoint.h>

#include <imgui.h>

#include "EditorLayer.h"

namespace Mahakam
{
	class Editor : public Application
	{
	public:
		Editor() : Application({ "Erebor", "assets/textures/internal/icon.png" })
		{
			PushOverlay(new EditorLayer());

			//getWindow().setVSync(true);
		}

		~Editor()
		{

		}
	};

	extern Application* Mahakam::CreateApplication()
	{
		return new Editor();
	}
}