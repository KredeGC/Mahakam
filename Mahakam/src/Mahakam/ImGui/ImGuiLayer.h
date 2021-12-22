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

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImGuiRender() override;

		void begin();
		void end();
	};
}