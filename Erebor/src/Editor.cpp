#include <Mahakam.h>
#include <Mahakam/Core/EntryPoint.h>

#include <imgui.h>

#include "DockLayer.h"
#include "StatsLayer.h"
#include "ProfilerLayer.h"
#include "SceneViewLayer.h"

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
			pushOverlay(new DockLayer());
			pushOverlay(new SceneViewLayer(&drawCalls, &vertexCount, &triCount));
			pushOverlay(new StatsLayer(&drawCalls, &vertexCount, &triCount));
			pushOverlay(new ProfilerLayer());

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