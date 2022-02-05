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
		Editor() : Application("Erebor")
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

	extern void Mahakam::InitRenderPasses(std::vector<RenderPass*>& renderPasses, uint32_t width, uint32_t height)
	{
		renderPasses.emplace_back(new GeometryRenderPass(width, height));
		renderPasses.emplace_back(new LightingRenderPass(width, height));
		renderPasses.emplace_back(new TonemappingRenderPass(width, height));
	}
}