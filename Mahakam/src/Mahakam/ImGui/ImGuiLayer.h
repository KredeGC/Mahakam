#pragma once

#include "Mahakam/Core/Layer.h"

#include "Mahakam/Events/MouseEvent.h"
#include "Mahakam/Events/KeyEvent.h"
#include "Mahakam/Events/ApplicationEvent.h"

namespace Mahakam {
	class ImGuiLayer : public Layer
	{
	private:
		float m_Time = 0;

		bool m_BlockEvents = false;

		ImFont* m_MonoFont;

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& event) override;

		void BlockEvents(bool block) { m_BlockEvents = block; }

		ImFont* GetMonoFont() const { return m_MonoFont; }

		void Begin();
		void End();
	};
}