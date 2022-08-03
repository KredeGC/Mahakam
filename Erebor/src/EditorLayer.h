#pragma once

#ifndef MH_RUNTIME
#include "Panels/DockSpace.h"
#endif

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class EditorLayer : public Layer
	{
	private:
#ifndef MH_RUNTIME
		DockSpace m_DockSpace;
#endif

		Scope<SharedLibrary> m_Runtime;

	public:
#ifndef MH_RUNTIME
		EditorLayer() : Layer("Editor"), m_DockSpace() {}
#else
		EditorLayer() : Layer("Editor") {}
#endif

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		void CopyRuntime(std::istream& binaryStream, size_t binaryLength);
		void UpdateRuntimeLibrary();

		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnWindowResized(WindowResizeEvent& event);
	};
}