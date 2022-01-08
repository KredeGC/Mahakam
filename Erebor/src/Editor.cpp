#include "ebpch.h"
#include <Mahakam.h>
#include <Mahakam/Core/EntryPoint.h>

#include <imgui.h>

#include "EditorLayer.h"

namespace Mahakam
{
	class Editor : public Application
	{
	private:
		uint32_t drawCalls = 0;
		uint32_t vertexCount = 0;
		uint32_t triCount = 0;

	public:
		Editor() : Application("Erebor")
		{
			pushOverlay(new EditorLayer(&drawCalls, &vertexCount, &triCount));

			//getWindow().setVSync(true);
		}

		~Editor()
		{

		}
	};

	extern Application* Mahakam::createApplication()
	{
		return new Editor();
	}
}