#pragma once

#include "Core.h"
#include "Mahakam/Core/Timestep.h"
#include "Mahakam/Events/Event.h"

namespace Mahakam
{
	class Layer
	{
	protected:
		std::string debugName;

	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(Timestep dt) {}
		virtual void onImGuiRender() {}
		virtual void onEvent(Event& event) {}

		inline const std::string& getName() const { return debugName; }
	};
}